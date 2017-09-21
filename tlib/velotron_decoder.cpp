
#ifdef WIN32
	#include <windows.h>
#else
	#ifdef __MACH__
		#include <minmax.h>
	#else
		#include <minmax.h>
	#endif
#endif

#include <math.h>
#include <assert.h>

#include <glib_defines.h>
#include <round.h>
#include <tdefs.h>
#include <tglobs.h>
#include <beepsound.h>
#include <velotron_decoder.h>

/*********************************************************

*********************************************************/

#ifndef NEWUSER
velotronDecoder::velotronDecoder(Course *_course, User *_user, Bike *_bike, Physics *_physics, int _id)  : Decoder(72, _course, _user, _id)  {
#else
velotronDecoder::velotronDecoder(Course *_course, RIDER_DATA &_rd, Bike *_bike, Physics *_physics, int _id)  : Decoder(72, _course, _rd, _id)  {
#endif

	physics = _physics;
	lastTdcPacket = 0;
	packets = 0;
	dbg = false;
	course_is_3dc = false;
	can_do_drag_factor = true;
	period_timeout = 0L;
	max_period_timeout = 0L;
	//deccels = 0L;

	TICK_PERIOD = 5.0f;
	MINDLY = ROUND((100.0f / TICK_PERIOD));			// minimum period 100ms, 1/2 turn @ 300RPM
	MAXDLY = ROUND((2500.0f / TICK_PERIOD));			// max ramp time, 1 turn @ 24RPM

	msperiod = 0L;

#if 0
	if (course)  {
		const char *cptr = course->get_filename();
		//cptr = course->get_filename();
		int pos;
		pos = indexIgnoreCase(cptr, ".3dc");
		if (pos != -1)  {
			course_is_3dc = true;
		}
	}
#endif
	

	gp.front = 0;
	gp.rear = 0;
	polarsig = 0;
	polarStrength = 0;
	elStrength = 0;
	d0 = d1 = d2 = 0;
	flags = 0;
	dms = ms = 0;
	msperiod = 0L;
	lastmph = 0;
	hrPluggedIn = 0;
	hisig = losig = 0;
	amps = 0;
	polarbit = 0;
	voltsCount = 0;
	supplyVoltage = 0;
	bp = 0;
	caltime = 0;

	status = 0;
	dt = 0;
	memset(&meta, 0, sizeof(meta));

	if (bini)  {
		ini = new Ini("decoder.ini");
	}
	else  {
		ini = NULL;
	}


#ifdef _DEBUG
	totalmcur = 0;
#endif

	bike = _bike;

	hr = new Heartrate();

	if (ini)  {
		debug = ini->getBool("options", "debug", "false", false);
	}
	else  {
		debug = false;
	}



	if (ini)  {
		Vdd = ini->getFloat("options", "Vdd", "5.0", false);
	}
	else  {
		Vdd = 5.0;
	}

	if (ini)  {
		logging = ini->getBool("options", "logging", "false", false);
	}
	else  {
		logging = false;
	}
	if (logging)  {
		ini->writeBool("options", "logging", false);		// logging only for this session, must be re-enabled manually
	}

	assert(sizeof(NVRAM)==256);
	memset(nvram, 0, sizeof(nvram));

	earLobeSSM = new signalStrengthMeter(200);
	polarSSM = new signalStrengthMeter(200);
	polarFilter = new LPFilter(50);

	int itemp;
	if (ini)  {
		itemp = ini->getInt("options", "volts filter", "50", false);
	}
	else  {
		itemp = 50;
	}
	voltsFilter = new LPFilter(itemp);

#ifndef NEWUSER
	if (user)  {
		lbs = user->data.lbs;
		meta.minhr = (unsigned short) (.5 + user->data.lower_hr);
		meta.maxhr = (unsigned short) (.5 + user->data.upper_hr);
	}

#else
	lbs = (float)(rd.pd.kgs * TOPOUNDS);
	meta.minhr = (unsigned short) (.5 + rd.lower_hr);
	meta.maxhr = (unsigned short) (.5 + rd.upper_hr);
#endif

#if 0
	if (course)  {
		switch(course->type)  {
			case Course::ERGO:
				totalLen = course->getTotalLen();		// totalLen is in minutes
				totalMS = (DWORD) (totalLen*60*1000);
				break;
			case Course::THREED:
				totalLen = (float) (course->getTotalLen() / 5280.0);		// get total len in miles
				break;
			case Course::NONERGO:
				totalLen = (float) (course->getTotalLen() / 5280.0);		// totalLen is in miles
				break;
			case Course::EMPTY:		// spinscan, manual charts, 3d have no course
				totalLen = 0;
				break;
			default:
				throw(fatalError(__FILE__, __LINE__));
				break;
		}
	}
	else {
		totalLen = 0.0f;
	}
#else
		total_miles = 0.0f;
#endif

	meta.gear = (unsigned char) (.5 + bike->state.gear_inches);
	meta.virtualFrontGear = bike->state.gp.front;
	meta.virtualRearGear = bike->state.gp.rear;

	if (logg)  {
		logg->write("constructor\n");
	}

}

/*********************************************************

*********************************************************/

velotronDecoder::~velotronDecoder(void)  {
	DEL(ini);
	bike = NULL;
	DEL(hr);
	DEL(earLobeSSM);
	DEL(polarSSM);
	DEL(polarFilter);
	DEL(voltsFilter);

	bp = 0;
}

/**********************************************************************

	hole period is 18 bits

format 0 packet:
----------------

0	1fff hhhh		1 = sync bit
						fff = format (000)
						hhhh = 4 ms bits of hole period (bits 14-17)

1	0hhh hhhh		next 7 bits of hole period (7-13)

2	0hhh hhhh		lsb 7 bits of hole period (0-6)


3	0zzz zzzz		lower 7 bits of high gain hr

4	0zzz zzzz		lower 7 bits of mcur

5	0zzz zzzz		lower 7 bits of volts

6	0vvv Uiii		U = unused
						vvv = 3 upper bits of volts
						iii = 3 upper bits of current

7	0hhh ssce		ss = rocker switch setting
								00 = middle
								01 = run
								10 = braking

						hhh = 3 upper bits of high gain hr

						c = crank detected bit
						e = ear lobe sensor not plugged in bit


format 1 packet:
----------------

0	1fff uuuu		1 = sync bit
						fff = packet format:
								001 = calibration timer
						uuuu = unused

1 0zzz zzzz			ms byte of 'timer'
2 0zzz zzzz			next byte of 'timer'
3 0zzz zzzz			next byte of 'timer'
4 0zzz zzzz			ls byte of 'timer'
5 0uuu hhhh			h = high bits of timer bytes

format 2 packet (exception packet):
-----------------------------------

0	1FFF nnnn		1 = sync bit
						FFF = packet format:
								010 = exception

						nnnn = exception number (0 - 15)

						0000 = breakpoint packet ("got here")


**********************************************************************/


/*********************************************************************************
	10 bit version

	hole period is 18 bits

format 0 packet:
----------------

if pc sends debug command the debug bit is turned on (packet[6], bit 3)

if debug bit is on
then the 10 bit volts is replaced with 9 bit ram address and
10 bit high gain hr is replaced with 8 bit ram data. This allows the pc to
see our ram in "real time": all 512 bytes can be sent in .005 * 512 = 2.56 secs.

0	1FFF hhhh		1 = sync bit
						FFF = packet format:
								000 = regular data packet
						hhhh = 4 ms bits of hole period (bits 14-17)

1	0hhh hhhh		next 7 bits of hole period (7-13)

2	0hhh hhhh		lsb 7 bits of hole period (0-6)

#if (debug)
3	0zzz zzzz		lower 7 bits ram data
#else
3	0zzz zzzz		lower 7 bits of high gain hr
#endif

4	0zzz zzzz		lower 7 bits of mcur

#if (debug)
5	0zzz zzzz		lower 7 bits of ram address
#else
5	0zzz zzzz		lower 7 bits of volts
#endif

6	0vvv fiii		f = debug (ram dump) flag
#if (debug)
						vvv = 3 upper bits of ram address (if debug flag is on)
#else
						vvv = 3 upper bits of volts
#endif
						iii = 3 upper bits of current

7	0hhh ssce		ss = rocker switch setting
								00 = middle
								01 = run
								10 = braking

#if (debug)
						hhh = 1 upper bit of ram data
#else
						hhh = 3 upper bits of high gain hr
#endif

						c = crank detected bit
						e = ear lobe sensor not plugged in bit

8 0bbb bbbh			h = polar heartrate bit (PB3)
						b = bits 1-6 of keypad (ls bit is key 1 (0x02))

9 0ccc cccc			c = 7 bit checksum


format 1 packet:
----------------

0	1FFF uuuu		1 = sync bit
						FFF = packet format:
								001 = calibration timer
						uuuu = unused

1 0zzz zzzz			ms byte of 'timer'
2 0zzz zzzz			next byte of 'timer'
3 0zzz zzzz			next byte of 'timer'
4 0zzz zzzz			ls byte of 'timer'
5 0uuu hhhh			h = high bits of timer bytes
6 0000 0000
7 0000 0000
8 0000 0000
9 0ccc cccc			c = 7 bit checksum


format 2 packet (exception packet):
-----------------------------------

0	1FFF nnnn		1 = sync bit
						FFF = packet format:
								010 = exception

						nnnn = exception number (0 - 15)

						0000 = breakpoint packet

1 0000 0000
2 0000 0000
3 0000 0000
4 0000 0000
5 0000 0000
6 0000 0000
7 0000 0000
8 0000 0000
9 0ccc cccc			c = 7 bit checksum



*********************************************************************************/

int velotronDecoder::decode(unsigned char *packet, DWORD _ms)  {

	unsigned char format;
	int i;
	unsigned char mask;
	unsigned char thistdc;


	packets++;

	#ifdef _DEBUG
	//avgtimer->update();					// 5.00 ms
	#endif

	format = packet[0] & 0x70;

	switch(format)  {
		case 0x00:  {

			d0 = packet[0] & 0x0000000f;		// bits 14-17
			d1 = packet[1] & 0x0000007f;		// bits 7-13
			d2 = packet[2] & 0x0000007f;		// bits 0-6

			d0 = d0 << 14;						// move to position 14
			d1 = d1 << 7;						// move to position 7
			msperiod = d0 | d1 | d2;	// this is just the pic counter

			//physics->period = period;

			if (msperiod != 0)  {
				period_timeout = 0L;
				if (physics) meta.mph = (float)physics->mph;
				meta.rawRPM = meta.rpm = (float) (bike->state.K1 / msperiod);		// K1 is function of gear ratio!!
				rawspeed = 50.0f*meta.mph / 8000.0f;
				//accum_tdc = get_rpm(tdc, &accum_rpm);
			}
			else  {
				//zzz
				if (started)  {
					period_timeout++;
					max_period_timeout = MAX(max_period_timeout, period_timeout);			// 511 = 2.55 seconds
				}

				if (period_timeout>=511)  {
					meta.mph = (float) lastmph;
					meta.rpm = 0.0f;						// tlm20011114
				}
			}

			// One count in the hole period corresponds to a time increment of 
			// 1/(460812.5) = 2.17008002 micro seconds.

			hrPluggedIn = (packet[7] & 0x01) == 1;

			thistdc = ((packet[7] & 0x02) == 0x02);

			DWORD spr;

			meta.tdc = false;
			if (thistdc)  {
				accum_tdc = 1;
				spr = packets - lastTdcPacket;
//xxx
				lastTdcPacket = packets;

				if (spr > 20.0f)  {
					meta.pedalrpm = (float) ((60.0f * (200.0f / (float)spr)));
					meta.tdc = true;
					//if (meta.rawRPM==0.0f)  {
					//	bp = 1;
					//}
				}

			}
			else if ( (packets - lastTdcPacket) > 600)  {			// 20 rpm cutoff     200 * 60/cutoff_rpm
				// packets come in at 200 per second, so if it has been 400 packets since the last tdc
				// (30 seconds) assume they quit peddling
				meta.pedalrpm = 0.0f;
				/*
				meta.rawRPM = meta.rpm = 0.0f;
				physics->rpmFilter->setfval(0.0f);
				*/
			}

			compute_armed_rpm();								// motion sensor version

			if ( packet[6] & 0x08)  {
				dbg = true;
			}

			/*
			if debug bit is on
			then the 10 bit volts is replaced with 9 bit ram address and
			10 bit high gain hr is replaced with 8 bit ram data. This allows the pc to
			see our ram in "real time": all 512 bytes can be sent in .005 * 512 = 2.56 secs.
			*/

			//-----------
			// heartrate
			//-----------

			if(!dbg)  { 
				hisig = packet[3];									// high gain hr
				hisig |= ((packet[7] & 0x70) << 3);			// get high bits of current
			}
			else  {
				ramdata = packet[3];
				ramdata |= ((packet[7] & 0x10) << 3);
			}

			//-----------
			// current
			//-----------


			meta.mcur = packet[4];									// lower 7 bits of measured current
			meta.mcur |= ((packet[6] & 0x07) << 7);			// get high bits of current

			amps = meta.mcur * 3.0/960.0;					// resolution is 3/240 = .0125 amps

			#ifdef _DEBUG
			totalmcur += meta.mcur;
			#endif

			//-----------
			// voltage
			//-----------

			if (!dbg)  {
				voltsCount = packet[5];
				voltsCount |= ((packet[6] & 0x70) << 3);			// get high bits of voltage
				voltsCount = (unsigned short) (.5 + voltsFilter->calc((double)voltsCount));
				meta.volts = voltsCount;
				supplyVoltage = ((4.99+1.0)/1.0)*(((double)voltsCount / 1023.0) * Vdd);
			}
			else  {
				ramaddr = packet[5];
				ramaddr |= ((packet[6] & 0x30) << 3);			// get high bits of address
				//ram[ramaddr] = ramdata;
			}


			//	zzzz 00xx = middle
			//	zzzz 01xx = run
			//	zzzz 10xx = braking

			//-----------------
			// rocker switch
			//-----------------

			if (packet[7] & 0x04)  {
				meta.runSwitch = true;
			}
			else if (packet[7] & 0x08)  {
				meta.brakeSwitch = true;
			}

			/***********************************************************************


				1      2     3



            4      5     6



				0 k k k   k k k p
				| | | |   | | | |
				| | | |   | | |  -------- polar bit
				| | | |   | | |
				| | | |   | | |
				| | | |   | |  ---------- key 1, start -----------------
				| | | |   |  ------------ key 2, grade up --            |
				| | | |    -------------- key 3, gear up    | reset     | pause/resume
				| | |                                       |           |
				| |  ---------------- key 4, shift key ---x-----------
				|  ------------------ key 5, grade down
                 -------------------- key 6, grade up

				1 = start key
				2 = grade up
				3 = gear up
				4 = shift key
				5 = grade down
				6 = gear down

				4-1 = pause / resume
				4-2 = reset
				4-3 = brake

				KEY		code
				1		0x02
				2		0x04
				3		0x08
				4		0x10
				5		0x20
				6		0x40

			***********************************************************************/



			if ( (packet[8] & 0x01)==0x01)  {
				polarbit = 1;
			}
			else  {
				polarbit = 0;
			}


			meta.keys = packet[8];			// raw key data
			mask = 0x02;


			for(i=0;i<6;i++)  {
				if (mask & meta.keys)  {
					if ((mask & lastkeys)==0)  {
						meta.keydown |= mask;
					}
				}
				else  {
					if (mask & lastkeys)  {
						meta.keyup |= mask;
					}
				}
				mask <<= 1;
			}

			lastkeys = meta.keys;
			keys = meta.keys;
			accum_keys |= keys;		// save the current key presses along with all key presses since accum_keys was last cleared

			if (hr) {

				/*
				 hisig goes from 0 to 1023 (10 bits), with '0' being at 512, and the
				 average being 512. We need to detect whether there is modulation on
				 this signal, and if not, use the polar signal.

				 If the earlobe sensor plugged in the signal strength rises to about 13,
				 centered at 512.

				 If it is unplugged, the signal strength is about 4.5, centered on 512

				*/


				elStrength = earLobeSSM->compute((double)hisig);

				bool polar_found = false;

				if (polarbit)  {
					polarsig = 128.0f + 150.0f;
					hrvalid = true;
					polar_found = true;
				}
				else  {
					polarsig = 128.0f - 10.0f;
				}
				polarsig = (float) polarFilter->calc((double)polarsig);				// will ramp up from 0 to 118 with no signal
				polarStrength = polarSSM->compute((double)polarsig);


				if (elStrength < 7.5)  {			// no earlobe sensor plugged in
					if (polarStrength > 1.0)  {
						hr->input(0, (unsigned short) (.5 + polarsig) );
					}
					else  {
						hr->filteredppm = 0;
					}
					if (!polar_found)  {
						hrvalid = false;			// don't override the polar hrvalid setting
					}
				}
				else  {
					hrvalid = true;
					hr->input(losig, hisig);		// pass in the raw voltages
				}
				meta.hr = (float) hr->filteredppm;
				if (hrvalid)  {
					meta.hrflags = 0x00;
				}
				else  {
					meta.hrflags = 0x0c;
				}
#ifdef _DEBUG
				if (meta.hr > 10.0f)  {
					bp = 1;
				}
#endif
			}
			else  {
				throw(fatalError(__FILE__, __LINE__));
			}


			//---------------
			// run physics:
			//---------------

			if (physics)  {
				physics->rpmFromHoles = meta.rpm;		// flywheel rpm
				physics->rpmFromPedals = meta.pedalrpm;
				physics->mcur = meta.mcur;		// need this for the control loop!!!!

				physics->grade = meta.grade;			// tlm2008-08-20 added


				physics->run();					// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
				meta.rpm = (float) physics->frpm;
				meta.mph = (float) physics->fmph;
				meta.calories = (float) physics->Calories;
				meta.rawAcceleration = (float) physics->rawAcceleration;
				meta.faccel = (float) physics->faccel;
				meta.miles = (float) physics->miles;
				meta.feet = (float) (5280.0 * meta.miles);

				meta.time = physics->ms;
			}

			meta.minutes = (float) (meta.time / (1000.0 * 60.0));

			/*
			// use acceleration to determine if we've quit pedaling

			if (meta.faccel <= 0.0f)  {
				deccels++;
				if (deccels > 200)  {				// at 5 ms this is a 1 second timeout
					rpm
				}
			}
			else  {
				decels = 0L;
			}
			*/

#if 0
			if (course)  {
				switch(course->type)  {
					case Course::ERGO:  {
						meta.load = (float) course->getLoad((double)meta.minutes);					// get the watts load from the course

						if (started)  {
							if (!finished)  {
								physics->setConstantWatts(meta.load);
							}
						}

						if (started)  {
							meta.watts = (float) physics->displayedWatts;
						}
						else  {
							meta.watts = (float) physics->displayedWatts;
						}
						break;
					}
					case Course::THREED:
						physics->grade = course->getGrade(physics->miles);
						// physics->wind is in MPH.
						// if course is a .3dc course then course->getWind(miles) returns kph
						if (course_is_3dc)  {
							physics->wind = KPHTOMPH*course->getWind(physics->miles);
						}
						else  {
							physics->wind = course->getWind(physics->miles);
						}
						meta.grade = (float) physics->grade;
						meta.wind = (float) physics->wind;
						meta.watts = (float) physics->displayedWatts;
						break;

					case Course::EMPTY:  {				// spinscan running from a recorded charts file, eg
						physics->grade = course->getGrade(physics->miles);
						physics->wind = course->getWind(physics->miles);
						meta.grade = (float) physics->grade;
						meta.wind = (float) physics->wind;
						break;
					}
					case Course::NONERGO:
					default:
						physics->grade = course->getGrade(physics->miles);
						physics->wind = course->getWind(physics->miles);
						meta.grade = (float) physics->grade;
						meta.wind = (float) physics->wind;
						meta.watts = (float) physics->displayedWatts;			// what the public sees
						break;
				}				// switch (course->type)
			}					// if (course)
#endif
			


			if (physics)  {
				meta.watts = (float) physics->displayedWatts;
			}

			accum_watts += meta.watts;
			accum_watts_count++;

//#ifdef _DEBUG
#if 0
			if (accum_watts > 0.01f)  {
				bp = 5;
			}
#endif

//xxx
			accum_rpm += meta.rpm;
			accum_rpm_count++;

			accum_hr += meta.hr;
			accum_hr_count++;

			accum_kph += (float) (TOKPH*meta.mph);
			accum_kph_count++;

#ifndef NEWUSER
			if (user)  {
				wpkg = (float) (meta.watts / (TOKGS*user->data.lbs));
			}
			else  {
				wpkg = 1.0f;
			}
#else
			wpkg = (float) (meta.watts / rd.pd.kgs);
#endif

			if (ss)  {
#if 0
				if (course)  {
					if (course->type == Course::ERGO)  {

						//-------------------------------------------------------------------------
						//	ss->totalss = meta.watts/physics->rpmFromHoles
						//	we want to amplify the modulation to get the spinscan numbers to agree
						//	with the non-ergo spinscan numbers. So we need to amplify the rpm 
						//	modulation.
						//-------------------------------------------------------------------------

						ss->run(physics->watts, physics->force93, physics->rpmFromHoles, meta.tdc);		// make sure to use the unfiltered watts for spinscan!!!

						/*
						meta.ss = ss->totalss;
						meta.lss = ss->leftss;
						meta.rss = ss->rightss;
						meta.lwatts = ss->leftwatts;
						meta.rwatts = ss->rightwatts;
						meta.lata = (int) (.5 + ss->lata);
						meta.rata = (int) (.5 + ss->rata);
						*/
					}
					else  {
						float f = ss->run(physics->watts, physics->force93, physics->rpmFromHoles, meta.tdc);		// make sure to use the unfiltered watts for spinscan!!!
						/*
						meta.ss = ss->totalss;
						meta.lss = ss->leftss;
						meta.rss = ss->rightss;
						meta.lwatts = ss->leftwatts;
						meta.rwatts = ss->rightwatts;
						meta.lata = iround(ss->lata);
						meta.rata = iround(ss->rata);
						*/
					}
					/*
					// spinscan, manual watts, and 3d have no course!
					ss->run(physics->watts, physics->force93, physics->rpmFromHoles, meta.tdc);		// make sure to use the unfiltered watts for spinscan!!!
					meta.ss = ss->totalss;

					meta.lss = ss->leftss;
					meta.rss = ss->rightss;
					meta.lwatts = ss->leftwatts;
					meta.rwatts = ss->rightwatts;
					meta.lata = iround(ss->lata);
					meta.rata = iround(ss->rata);
					meta.rescale = ss->rescale;
					*/
				}								// if (course)
				else  {
					float f = ss->run(physics->watts, physics->force93, physics->rpmFromHoles, meta.tdc);		// make sure to use the unfiltered watts for spinscan!!!
					/*
					meta.ss = ss->totalss;
					meta.lss = ss->leftss;
					meta.rss = ss->rightss;
					meta.lwatts = ss->leftwatts;
					meta.rwatts = ss->rightwatts;
					meta.lata = iround(ss->lata);
					meta.rata = iround(ss->rata);
					*/
				}
#endif
				// spinscan, manual watts, and 3d have no course!
				if (physics)  {
					ss->run(physics->watts, physics->force93, physics->rpmFromHoles, &meta);		// make sure to use the unfiltered watts for spinscan!!!
				}

				/*
				meta.ss = ss->totalss;
				meta.lss = ss->leftss;
				meta.rss = ss->rightss;
				meta.lwatts = ss->leftwatts;
				meta.rwatts = ss->rightwatts;
				meta.lata = iround(ss->lata);
				meta.rata = iround(ss->rata);
				meta.rescale = ss->rescale;
				*/
			}									// if (ss)


			computePP();		// needs valid hr & watts

			//------------------------------------
			// end of velotron-specific things
			//------------------------------------

			if (started)  {
				if (!finished)  {
					if (!paused)  {
						if (formula) formula->calc(meta.watts);


//						ss->computeAverages(&meta);		// updates ss averages every 100 ms
						peaks_and_averages();

						meta.miles = (float) physics->miles;
						meta.feet = (float) (meta.miles*5280.0);
						meters = (float) (MILESTOMETERS * meta.miles);

#if 0
							switch(course->type)  {
								case Course::ERGO:
									if (meta.time >= totalMS)  {
										meta.time = totalMS;
										finished = true;
										finishEdge = true;
#ifdef WIN32
										//MessageBeep(MB_ICONASTERISK);
#endif
										physics->pause();			// stops physics
									}
									break;
								case Course::THREED:
								case Course::NONERGO:
									if (physics->miles >= totalLen)  {
										meta.miles = totalLen;	// tlm20030812, last logging is EXACTLY totalLen
										finished = true;
										finishEdge = true;
#ifdef WIN32
										//MessageBeep(-1);
#endif
										physics->pause();			// stops physics
									}
									break;

								default:
									break;
							}
#endif

					}				// if (!paused)
				}					// if (!finished)
				else  {
					finishEdge = false;
					meta.miles += (float) (meta.mph*(dt/3600.0));			// integrate the speed to get miles
					meta.feet = (float) (meta.miles*5280.0);
					meters = (float) (MILESTOMETERS * meta.miles);
				}
			}						// if (started)

			runkeys();

			break;
		}							// case 0x00:


		case 0x10:  {						// format 1, calibration results
			/****
			format 1 packet:
			----------------

			0	1fff uuuu		1 = sync bit
									fff = packet format:
											001 = calibration timer
									uuuu = unused
			1 0zzz zzzz			ms byte of 'timer'
			2 0zzz zzzz			next byte of 'timer'
			3 0zzz zzzz			next byte of 'timer'
			4 0zzz zzzz			ls byte of 'timer'
			5 0uuu hhhh			h = high bits of timer bytes
			****/

			caltime = (packet[1] << 24);
			caltime |= (packet[2] << 16);
			caltime |= (packet[3] << 8);
			caltime |= packet[4];
			if ((packet[5] & 0x08) == 0x08)  {
				caltime |= 0x80000000;
			}
			if ((packet[5] & 0x04) == 0x04)  {
				caltime |= 0x00800000;
			}
			if ((packet[5] & 0x02) == 0x02)  {
				caltime |= 0x00008000;
			}
			if ((packet[5] & 0x01) == 0x01)  {
				caltime |= 0x00000080;
			}

			break;
		}									// case 0x10

		case 0x20:  {					// general information packet
			/*

			contains 7 bytes of information in 16 different formats

			format 2 packet:
			----------------
			0 1fff zzzz				1 = sync bit
									fff = packet format:
											010 = information packet
									zzzz = bit flags
									0000 = unsolicited exception
									0001 = nvram format (1st data byte is starting nvram address)
									0010
									0011
									0100
									0101
									0110
									0111
									1000
									1001
									1010
									1011
									1100
									1101
									1110
									1111

			1 0zzz zzzz			lower 7 bits of data
			2 0zzz zzzz			lower 7 bits of data
			3 0zzz zzzz			lower 7 bits of data
			4 0zzz zzzz			lower 7 bits of data
			5 0zzz zzzz			lower 7 bits of data
			6 0zzz zzzz			lower 7 bits of data
			7 0zzz zzzz			lower 7 bits of data

			8 0zzz zzzz			upper bits of above data

			*/

			unsigned char type = packet[0] & 0x0f;
			unsigned char data[7];

			// unpack the data:

			for(i=0;i<7;i++)  {
				data[i] = packet[i+1];
				packet[8] <<= 1;
				if (packet[8] & 0x80)  {
					data[i] |= 0x80;
				}
			}

			switch(type)  {
				case 0:						// unsolicted exception
					break;

				case 1:  {						// nvram
					// gets 6 bytes of nvram every 5 ms. Total time to get nvram = .21 sec
					//data[0] is the starting address
					int k;
					for(i=0; i<6; i++)  {
						k = data[0] + i;
						if (k>255)  {
							break;
						}
						nvram[k] = data[i+1];
					}
					break;
				}
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
					break;
			}

			break;
		}								// case 0x20:

		case 0x30:  				// available
		case 0x40:  				// available
		case 0x50:  				// available
		case 0x60:  				// available
		case 0x70:  				// available
		default:  {
			if (logg)  {
				logg->write(10,0,1,"error: packet format = %02x\n", format);
				logg->dump(packet, PACKETLEN);
				logg->write(10,0,1,"\n");
			}

			#ifdef _DEBUG
				//MessageBox(NULL, "unknown packet (see decoder.log)", "error", MB_OK);
			#endif

			break;
		}
	}



	return 0;
}						// decode()

/*************************************************************************

*************************************************************************/

void velotronDecoder::runkeys(void)  {


	if (keydownfunc)  {
		switch(meta.keydown)  {

			case KEY1:
				#ifdef LOGVELKEYS
					logg->write("\nkey1 down\n");
				#endif
				keydownfunc(object, id, 1);
				break;

			case KEY2:
				#ifdef LOGVELKEYS
					logg->write("\nkey2 down\n");
				#endif
				keydownfunc(object, id, 2);
				break;

			case KEY3:
				#ifdef LOGVELKEYS
					logg->write("\nkey3 down\n");
				#endif
				keydownfunc(object, id, 3);
				break;

			case KEY4:
				#ifdef LOGVELKEYS
					logg->write("\nkey4 down\n");
				#endif
				shiftflag = true;
				keydownfunc(object, id, 4);
				break;

			case KEY5:
				#ifdef LOGVELKEYS
					logg->write("\nkey5 down\n");
				#endif
				keydownfunc(object, id, 5);
				break;

			case KEY6:
				#ifdef LOGVELKEYS
					logg->write("\nkey6 down\n");
				#endif
				keydownfunc(object, id, 6);
				break;

			case 0:														// call to allow key repeats (see ssv.cpp for example)
				keydownfunc(object, id, 0);
				break;

			default:									// multiple keys
				keydownfunc(object, id, meta.keydown);
				break;
		}

		meta.keydown = 0;
	}				// if (keydownfunc)  {


	if (keyupfunc)  {
		switch(meta.keyup)  {

			case KEY1:
				#ifdef LOGVELKEYS
					logg->write("key1 up\n");
				#endif
				keyupfunc(object, id, 1);
				break;

			case KEY2:
				#ifdef LOGVELKEYS
					logg->write("key2 up\n");
				#endif
				keyupfunc(object, id, 2);
				break;

			case KEY3:
				#ifdef LOGVELKEYS
					logg->write("key3 up\n");
				#endif
				keyupfunc(object, id, 3);
				break;

			case KEY4:
				#ifdef LOGVELKEYS
					logg->write("key4 up\n");
				#endif
				shiftflag = false;
				keyupfunc(object, id, 4);
				break;

			case KEY5:
				#ifdef LOGVELKEYS
					logg->write("key5 up\n");
				#endif
				keyupfunc(object, id, 5);
				break;

			case KEY6:
				#ifdef LOGVELKEYS
					logg->write("key6 up\n");
				#endif
				keyupfunc(object, id, 6);
				break;
					
			case 0:														// call to allow key repeats (see ssv.cpp for example)
				keyupfunc(object, id, 0);
				break;

			default:
				//logg->write(10,0,1,"ERROR: %d up\n", meta.keyup);
				break;
		}
	
		meta.keyup = 0;
	}					// 	if (keyupfunc)  {

	return;
}							// runkeys()

/*************************************************************************

*************************************************************************/

void velotronDecoder::set_grade(float _grade)  {

	physics->grade = _grade;
	return; 
}

/**********************************************************************************

**********************************************************************************/

void velotronDecoder::set_watts_factor(float _f)  { 

	Decoder::set_watts_factor(_f);
	if (physics)  {
		physics->set_watts_factor(_f);
	}
	return;
}

/**********************************************************************************

**********************************************************************************/

void velotronDecoder::set_weight_lbs(double _d)  {
	physics->set_weight_lbs(_d);
	return;
}

/*************************************************************************
		ds->decoder->meta.virtualFrontGear = bike->state.virtualFrontGear;
		ds->decoder->meta.virtualRearGear = bike->state.virtualRearGear;

*************************************************************************/

Bike::GEARPAIR velotronDecoder::get_gear_pair(void)  {
	gp.front = bike->state.gp.front;
	gp.rear = bike->state.gp.rear;
	return gp;
}

/*************************************************************************

*************************************************************************/

Bike::GEARPAIR velotronDecoder::get_gear_indices(void)  {
	gp.front = bike->state.params.front_ix;
	gp.rear = bike->state.params.rear_ix;
	return gp;
}

