
#include <float.h>
#include <assert.h>

#include <glib_defines.h>

#ifdef WIN32
	#include <minmax.h>
#else
#include <utils.h>						// timeGetTime()
#endif

#include <fatalerror.h>

#include <computrainer_decoder.h>

/*********************************************************

*********************************************************/

computrainerDecoder::computrainerDecoder(Course *_course, RIDER_DATA &_rd, int _id) : Decoder(48, _course, _rd, _id)  {
	int i;

	if (logg) logg->write(10,0,1,"computrainerDecoder: 1\n");
	tpf = new twoPoleFilter *[NBARS];

	for(i=0; i<24; i++)  {
		tpf[i] = new twoPoleFilter(100);		// argument is unused, so it doesn't matter
	}

	TICK_PERIOD = 30.3f;
	MINDLY = (int)(100/TICK_PERIOD+0.5f);        // 3
	MAXDLY = (int)(2500/TICK_PERIOD+0.5f);        // 228

	/*
	TICK_PERIOD = 30.3f;
	//TICK_PERIOD = 30.333333f;
	MINDLY = (100.0f / TICK_PERIOD);			// minimum period 100ms, 1/2 turn @ 300RPM
	MAXDLY = (2500.0f / TICK_PERIOD);			// max ramp time, 1 turn @ 24RPM
	*/

	#ifdef _DEBUG
		averagedms = 30;
	#endif

#ifdef _DEBUG
	//at = new AverageTimer("hbdecoder");
#endif

	if (logg) logg->write(10,0,1,"computrainerDecoder: 2\n");

	reset();

	if (logg) logg->write(10,0,1,"computrainerDecoder: x\n");

}

/*********************************************************

*********************************************************/

computrainerDecoder::~computrainerDecoder(void)  {
	if (tpf)  {
		for(int i=0; i<24; i++)  {
			DEL(tpf[i]);
		}
		delete [] tpf;
		tpf = NULL;
	}

#ifdef _DEBUG
	//DEL(at);
#endif

#ifdef _DEBUG
	sprintf(string, "average dms = %d\n", averagedms);
	OutputDebugString(string);
#endif

	bp = 0;
}


/***********************************************************************

	data comes in at 2400 baud about every 50 ms:

	12937191: 00 00 00 80 c9 ed 
	12937241: 00 00 00 80 10 00 
	12937291: 00 00 00 80 20 00 
	12937342: 00 00 00 80 3c 00 
	12937392: 00 00 00 80 40 00 
	12937442: 00 00 00 80 50 00 
	12937492: 00 00 00 80 68 00 
	12937542: 00 00 00 80 10 00 
	12937592: 00 00 00 80 70 00 
	12937642: 00 00 00 80 80 c7 
	12937692: 00 00 00 80 92 00 
	12937742: 00 00 00 80 a0 00 
	12937792: 00 00 00 80 ba 01 
	12937842: 00 00 00 80 c9 ed 
	12937892: 00 00 00 80 10 00 
	12937942: 00 00 00 80 20 00 


								RECEIVED PACKER FORMAT:

BYTE	FUNCTION
----	--------

0		spinscan
1		spinscan
2		spinscan

3		key presses
			bit 0		reset key			KEY1
			bit 1		f1 key				KEY4
			bit 2		f3 key				KEY5
			bit 3		plus key				KEY3
			bit 4		f2 key				KEY2
			bit 5		- key					KEY6
         ----------------------
			bit 6		spinscan sync
			bit 7		parity

4		high 4 bits	= id
			0x00				UNUSED?
			0x10				speed
         0x20				power
         0x30				low 8 bits = heart rate
         					high 4 bits = heart rate flags
			0x40				UNUSED?
         0x50				UNUSED?
			0x60				low 8 bits = rpm
         					high 4 bits = rpm flags
			0x70				low 8 bits = min heart rate
         					high 4 bits unused?
			0x80				low 8 bits = max heart rate
         					high 4 bits unused?
			0x90				rf drag
      	0xa0				UNUSED?
         0xb0				status
			0xc0				version
      	0xd0				UNUSED?
      	0xe0				UNUSED?
      	0xf0				UNUSED?


		low 4 bits	= high 4 bits of data

5		lower 8 bits of data



									data formats:

unsigned int	speed					mph*50
unsigned int	power					power in watts
unsigned int	heartrate			0 - 255

unsigned int	hrate_flags						BIT 0
											1 = blink heart
											0 = solid heart, display 0

														BIT 1
											1 = show "E" on display
											0 = don't

														BIT 2
											1 = below limit, show "LO" and beep slow
											0 = don't

														BIT 3
											1 = above "HI" limit, show "HI" and beep fast
											0 = don't

											if bits 2&3 set, no sensor is installed




unsigned int	load					unused
unsigned int	spinscan				unused
unsigned int	rpm					0 - 255

unsigned int	rpm_flags						BIT 3
											1 = rpm sensor plugged in
											0 = rpm sensor not plugged in


unsigned int	min_hrate			40 - 255, 0 = unset, no alarm
unsigned int	max_hrate			41 - 255, 0 = unset, no alarm
unsigned int	rf_drag				pounds*256
unsigned int	rf_measured			flag
unsigned int	io						unused
unsigned int	status				flags
unsigned int	version				??


SPINSCAN FORMAT:

	displayed timer
	rpm
	watts
	mph
	grade
	heartrate
	distance

	blue.ssraw[24]

entry:
	hb_buf[6] has latest raw data (already packed from the 7 byte rs232 packet).


***********************************************************************/


int computrainerDecoder::decode(unsigned char *packet, DWORD _ms)  {
	int i;
	unsigned short id, val;
	unsigned short vlow,vhi;
	int j;
	int rc = 0;
	

#ifdef _DEBUG
	//at->update();			// 30.29 ms
	//float prev_wind = meta.wind;
#endif

	keys = (unsigned char) (packet[3] & 0x3f);				// save the current key presses
	accum_keys |= (unsigned char) (packet[3] & 0x3f);		// save the current key presses along with all key presses since accum_keys was last cleared

#ifdef _DEBUG
	switch (keys) {
		case 0x01:				// reset
			bp = 1;
			break;
		case 0x02:				// f1
			bp = 1;
			break;
		case 0x04:				// f3
			bp = 1;
			break;
		case 0x08:				// +
			bp = 1;
			break;
		case 0x10:				// f2
			bp = 1;
			break;
		case 0x20:				// -
			bp = 1;
			break;
		case 0x00:
			bp = 3;
			break;
		default:
			bp = 2;
			break;
	}
#endif

	/*
		remap the keys for velotron-charts compatibility:

#define KEY1		0x02
#define KEY2		0x04
#define KEY3		0x08
#define KEY4		0x10
#define KEY5		0x20
#define KEY6		0x40

			bit 0		reset key			KEY1
			bit 1		f1 key				KEY4
			bit 2		f3 key				KEY5
			bit 3		plus key				KEY3
			bit 4		f2 key				KEY2
			bit 5		- key					KEY6

	newmask[0] = KEY1 = 0x02
	newmask[1] = KEY4 = 0x10
	newmask[2] = KEY5 = 0x20
	newmask[3] = KEY3 = 0x08
	newmask[4] = KEY2 = 0x04
	newmask[5] = KEY6 = 0x40

	*/

#ifdef _DEBUG
	//if (keys != 0)  {
	//	bp = 22;
	//}
#endif

#ifdef TDC_DECODER
	// assumes that the parity is ok for this byte


#if 0
	if (packet[3]&0x40)  {			// spinscan first bar sync flag
		//sscount = 0;
		//accum_tdc = 1;
		bp = 1;
	}
#endif

	if (packet[3] & 0x80)  {
		accum_tdc = 1;
		//bp = 1;
	}

	compute_armed_rpm();

	if (keys != lastkeys)  {
		unsigned char mask = 0x01;
		for(i=0; i<6; i++)  {
			if (mask & keys)  {
				if ((mask & lastkeys)==0)  {
					meta.keydown |= newmask[i];
				}
			}
			else  {
				if (mask & lastkeys)  {
					meta.keyup |= newmask[i];
				}
			}
			mask <<= 1;
		}

		lastkeys = keys;
		meta.keys = keys;
	}
#else
	if (packet[3]&0x40)  {			// spinscan first bar sync flag
		accum_tdc = 1;				// tlm20110822
	}

	unsigned char cc = packet[3];
	int cnt = 0;
	for(j=0; j<8; j++)  {
		if (cc&0x01)  {
			cnt++;
		}
		cc >>= 1;
	}
	if ((cnt%2)==0)  {
		//bad_parity_count++;
		packet_error3++;
		keys = 0;
		rc = 1;
	}
	else  {
		//if ((packet[3] & 0x80)==0)  {
		//	bp = 7;
		//}
		// odd parity
		if (keys != lastkeys)  {
			unsigned char mask = 0x01;
			for(i=0; i<6; i++)  {
				if (mask & keys)  {
					if ((mask & lastkeys)==0)  {
						meta.keydown |= newmask[i];
					}
				}
				else  {
					if (mask & lastkeys)  {
						meta.keyup |= newmask[i];
					}
				}
				mask <<= 1;
			}

			lastkeys = keys;
			meta.keys = keys;
		}
	}
#endif


#ifdef _DEBUG
	if (keys != 0)  {
		packet_error4++;
		rc = 1;
	}
#endif

	id = (unsigned short) (packet[4] & 0x00f0);
	vlow = (unsigned short) (packet[5] & 0x00ff);	// bc error
  	//vhi = (unsigned short) ((packet[4] & 0x0f) & 0x00ff);
  	vhi = (unsigned short) (packet[4] & 0x0f);
	val = (unsigned short) ((vhi << 8) | vlow);

	//----------------------------------------------
	// HANDLE THE SPINSCAN:
	// always update the output arrays for record()
	//----------------------------------------------

	if (packet[3]&0x40)  {			// spinscan first bar sync flag
		sscount = 0;
	}
	if (sscount<24 && packet[0])  {
		ssbuf[sscount++] = packet[0] ^ XORVAL;	// fix norm error in hb
		ssbuf[sscount++] = packet[1] ^ XORVAL;
		ssbuf[sscount++] = packet[2] ^ XORVAL;
		#ifdef _DEBUG
		//at->update();			// 247 ms
		#endif
	}
	if(sscount==24)  {		// got 8 blocks of 3 once every 2.00 seconds
		for(j=0; j<NBARS; j++)  {
			ss->thrust[j].raw = unpack((unsigned short)ssbuf[j]);
		}
		sscount++;				// to 25 for idle state
		#ifdef _DEBUG
		//at->update();			// 2000 ms
		#endif
	}

	//-------------------------------
	// HANDLE THE NON-SPINSCAN DATA:
	//-------------------------------

	switch(id)  {
  		case 0x10:													// mph
			#ifdef _DEBUG
				//at->update();			// 197 ms
			#endif

			meta.mph = val / 50.0f;

#if 0
	#ifdef _DEBUG
				/*
				if (manual)  {
					meta.mph = manual_mph;
					val = (unsigned short) ROUND(manual_mph * 50.0f);
				}
				else  {
					meta.mph = val / 50.0f;
				}
				*/
	#else
				meta.mph = val / 50.0f;
	#endif
#endif
			if (meta.mph > 99.0)  {
				meta.mph = 99.0;
			}
			mps = (MPHTOMPS * meta.mph);			// meters per second
			rawspeed = val / 8000.0f;

			accum_kph += (float)(TOKPH*meta.mph);
			accum_kph_count++;

  	      break;

      case 0x20:									// watts
			meta.watts = val;
			if (meta.watts > 3000.0f)  {
				meta.watts = 0.0f;
#ifdef _DEBUG
				throw(fatalError(__FILE__, __LINE__));
#endif
			}
			if (kgs > 0.0f)  {
				wpkg = meta.watts / kgs;
			}
			accum_watts += meta.watts;
			accum_watts_count++;
     	   break;

      case 0x30:										// heartrate
			/*
				BIT 0
					1 = blink heart
					0 = solid heart, display 0

				BIT 1
					1 = show "E" on display
					0 = don't

				BIT 2
					1 = below limit, show "LO" and beep slow
					0 = don't

				BIT 3
					1 = above "HI" limit, show "HI" and beep fast
					0 = don't

				if bits 2&3 set, no sensor is installed

			*/
			meta.hrflags = vhi;
			hrvalid = false;

			if ( (meta.hrflags & 0x02) == 0x02)  {				// error flag set
#ifdef _DEBUG
				bp = 3;
#endif
			}
			else if ( (meta.hrflags & 0x0c) == 0x0c)  {		// no heart rate sensor installed
#ifdef _DEBUG
				bp = 4;
#endif
			}
			else  {
				hrvalid = true;
			}
			if (hrvalid)  {
				meta.hr = vlow;
				accum_hr += meta.hr;
				accum_hr_count++;
			}
			else  {
				meta.hr = -1.0f;
				accum_hr = -1.0f;
				accum_hr_count = 1;
			}

        	break;

      case 0x60:									// rpm
			// note: 3d software will generate an rpm based on speed if there is no rpm signal
			meta.rpmflags = vhi;
			if (vhi & RPM_VALID)  {
				meta.pedalrpm = vlow;
				meta.rpm = vlow;
				meta.rpmValid = true;
				accum_rpm += meta.rpm;
				accum_rpm_count++;
				//rpm_invalid = 0;
			}
			else  {
				// no rpm sensor plugged in
				meta.rpm = -1.0f;
				meta.pedalrpm = 0;
				meta.rpmValid = false;
				accum_rpm = -1.0f;
				accum_rpm_count = 1;

				//rpm_invalid++;

				//if (rpm_invalid>3)  {
				//	rpm_invalid = 4;
				//}
			}
         break;

  	   case 0x70:
			meta.minhr = vlow;							// is this EVER non-zero?
     		break;

		case 0x80:
			meta.maxhr = vlow;							// is this EVER not 199?
			break;

		case 0x90:
			meta.rfdrag = (unsigned short) (val & 0x07ff);
			meta.rfmeas = (unsigned short) vhi;		// (vhi & 0x08);
#ifdef _DEBUG
			{
			//if (meta.rfdrag >350)  {
			//if (val > 0xbff)  {
				//unsigned short us = 0x7ff;
			float f = (float) meta.rfdrag / 256.0f;

			//if (val > 0xb73)  {
			//if (meta.rfdrag > 883)  {
			if (f > 4.0f)  {
				bp = 1;
			}
			if (meta.rfmeas >350)  {
				bp = 1;
			}
			}
#endif

  	      break;

      case 0xb0:
		  // 11		cadence pickup
		  // 10		heart rate sensor
		  //  9		pro or + version, 1 = pro
		  //  8		1 if drag factor is operating
		  //  7		0
		  //  6		0
		  //  5		0
		  //  4		0
		  //  3		0
		  //  2		0
		  //  1		0
		  //  0		1


	  	   	hbstatus = val;
     		break;

		case 0xc0:
			version = val;								// 2541, 4095, should be 4543

			switch(version)  {
				default:
				case 2541:								// pre drag factor rom
					can_do_drag_factor = false;
					break;
				case 4095:								// first drag factor rom
					can_do_drag_factor = true;
					break;
				case 3611:
					can_do_drag_factor = true;
					break;

			}
			val = (val%100);
			if (val <= 34)  {
				HB_ERGO_PAUSE = HB_ERGO_RUN;
			}
     	   break;

		case 0xa0:
			/*
			There are 4.576 holes per inch of travel, so with the gear range you have programmed into
			the velotron 105 to 440 inches/crank turn, the value will be from 480 to 2013.
			*/
			slip = val;				// unsigned short
			slipflag = true;
			break;

		case 0x50:
			raw_rpm = val;
			break;

		// 'id' is unused:

		case 0x00:
		case 0x40:
		case 0xd0:
		case 0xe0:
		case 0xf0:
			break;

      default:
			bp = 1;
  	   	break;
   }

	//-------------------------
	// get the time:
	//-------------------------

	if (packets==0)  {				// realtime OR filemode
		packets++;
		#ifdef _DEBUG
			averagedms = 30;
		#endif
		dms = 0;
		dt = 0.0;
		lastseconds = 0.0;
		ms = 0;
		lastms = 0;
		lastus = 0.0f;

		startTime = timeGetTime();

		#ifdef _DEBUG
		if (logg)
			//logg->write(10,0,1, "startTime = %ld\n", startTime);
		#endif

		pausedTime = 0;				// accumulates paused time
		pauseStartTime = 0;

	}
	else  {
		if (_ms == 0)  {					// realtime
#ifdef _DEBUG
			DWORD now = timeGetTime();
			ms = now - startTime - pausedTime;
			if (ms>1000)  {
				bp = 1;
			}
#else
			ms = timeGetTime() - startTime - pausedTime;
#endif
			if (lastms > ms)  {
				lastms = ms - 30;			// in case of a pause
			}
			packets++;

			dms = ms - lastms;
			dt = (double)dms / 1000.0;
			lastms = ms;
		}
		else  {							// filemode
			if (!paused)  {				// tlm20050312
				packets++;
				ms = _ms;					// passed in from file
				dms = ms - lastms;
				dt = (double)dms / 1000.0;
				lastms = ms;
			}
		}
	}

	// don't call from here because you can't get 100.00 ms timing in the function. Call AFTER dataSource::getNextRecord()
	// the average time between calls here is 30.3 ms so you can't hit 100 ms exactly.
	//ss->do_ten_hz_filter(&meta, started, finished, paused);			// computes ss peaks and averages
	#ifdef _DEBUG
		//at->update();			//  30.3 ms
	#endif

#ifdef _DEBUG
	//if (prev_wind != meta.wind)  {
	//	bp = 7;
	//}
#endif


	peaks_and_averages();

	if (!paused)  {
		wattseconds += (meta.watts*dt);
		meta.calories = (float)((wattseconds / 4186) / .26);		// see physics.cpp for formula derivation
	}


	if (started)  {
		if (!finished)  {
			if (!paused)  {
				meta.time = ms;
				meta.minutes = (float)meta.time / (60.0f*1000.0f);
				meta.miles += (float) (meta.mph*(dt/3600.0));			// integrate the speed to get miles
				meta.feet = (float) (meta.miles*5280.0);
				meters = (float)(MILESTOMETERS * meta.miles);

				//if (course)  {
				//	meta.wind = course->getWind(meta.miles);
				//}
				//meta.wind = 0.0f;											// tlm20110810

				//if (rider.ctb->ftp > 0.01f)  {
				if (formula)  {
					formula->calc(meta.watts);			// tlm20110616
				}
				//}
			}
		}
		else  {				// finished, but 3d needs distance to keep increasing so the rider goes past the finish line (cool down)
		}
	}

#ifdef _DEBUG
	//if (prev_wind != meta.wind)  {
	//	bp = 7;
	//}
#endif

	runkeys();


#ifdef _DEBUG
	//if (prev_wind != meta.wind)  {
	//	bp = 7;
	//}
#endif

	return rc;

}					// decode()


/**********************************************************************
	unpacks the spinscan numbers from the rs232 data
**********************************************************************/

float computrainerDecoder::unpack(unsigned short p)  {
	unsigned short n;
	float f;

	n = (short) (p & 0x07);				// n = exponent

	if (n)  {
		f = (float) (( (p&0xf8) | 0x100) << (n-1));
		return f/512.0f;
	}
	else  {
		f = (float) (p & 0xf8);
		return f/512.0f;
	}
}

/**********************************************************************

**********************************************************************/

void computrainerDecoder::reset(void)  {

	pausedTime = 0;
	pauseStartTime = 0;

	packets = 0;
	wattseconds = 0.0;
	lastms = 0;

	for(int i=0; i<24; i++)  {
		tpf[i]->reset();
	}

	Decoder::reset();			// sets runSwitch to FALSE

	ssFound = false;
	heartRateFlags = 0;
	minHeartRate = 0;
	maxHeartRate = 0;
	hbStatus = 0;
	version = 0;
	lastmphtime = 0;
	//lastFilterTime = 0;

	hb_DLE = 0xe7;
	HB_ERGO_RUN = 0x14;
	HB_ERGO_PAUSE = 0x10;
	sscount = 25;
	newmask[0] = KEY1;
	newmask[1] = KEY4;
	newmask[2] = KEY5;
	newmask[3] = KEY3;
	newmask[4] = KEY2;
	newmask[5] = KEY6;

#ifndef NEWUSER
	if (user)  {
		lbs = user->data.lbs;
		kgs = (float)(TOKGS * lbs);
	}
	else  {
		lbs = 0.0f;
		kgs = 0.0f;
	}
#else
	lbs = (float) (TOPOUNDS*rd.pd.kgs);
	kgs = (float)rd.pd.kgs;
#endif

	meta.runSwitch = 1;
	meta.lata = 90;
	meta.rata = 90;

	return;
}


/**********************************************************************

**********************************************************************/


void computrainerDecoder::peaks_and_averages(void)  {

	computePP();		// needs valid hr & watts

#if 1
	if (course)  {
		if (started)  {
			if (meta.miles >= total_miles)  {
				bp = 1;
				meta.miles = total_miles;
				if (!finished)  {
					finished = true;
					finishEdge = true;
					//log->write("%10ld: finishEdge = true\n", timeGetTime());
				}
				else  {
					// let the app clear the edge flag, since this class could clear it before the app sees it!!!
				}
			}
		}
	}
#endif

	if (!started)  {
		return;
	}
	if (finished)  {
		return;
	}
	if (paused)  {
		return;
	}

	//---------------------
	// compute averages:
	//---------------------

	/***********************************

	BIT 0
		1 = blink heart
		0 = solid heart, display 0
	BIT 1
		1 = show "E" on display
		0 = don't
	BIT 2
		1 = below limit, show "LO" and beep slow
		0 = don't
	BIT 3
		1 = above "HI" limit, show "HI" and beep fast
		0 = don't
	if bits 2&3 set, no sensor is installed

	************************************/

	/*
	hrvalid = false;

	if ( (meta.hrflags & 0x02) == 0x02)  {				// error flag set
	}
	else if ( (meta.hrflags & 0x0c) == 0x0c)  {		// no heart rate sensor installed
	}
	else  {
		hrvalid = true;
	}
	*/

	Decoder::peaks_and_averages();

	return;
}

/**********************************************************************

**********************************************************************/

void computrainerDecoder::gradeUp(void)  {
	meta.grade += .1f;
	if (meta.grade > MAXGRADE) meta.grade = MAXGRADE;
	return;
}

/**********************************************************************

**********************************************************************/

void computrainerDecoder::gradeDown(void)  {
	meta.grade -= .1f;
	if (meta.grade < MINGRADE) meta.grade = MINGRADE;
	return;
}

/**********************************************************************
	added for 3d software to stop jerking. called in between packets which
	come in at about every 33 ms.
**********************************************************************/

void computrainerDecoder::integrate(void) {

	if (!started)  {
		return;
	}

	if (paused)  {
		return;
	}

	ms = timeGetTime() - startTime - pausedTime;
		
	if (lastms > ms)  {
		lastms = ms - 30;			// in case of a pause
	}

	DWORD tdms = ms - lastms;

	// to stop calorie spikes?
	if (tdms > 500)  {
		tdms = 30;
	}

	double tdt = (double)tdms / 1000.0;
	lastms = ms;

	meta.miles += (float) (meta.mph*(tdt/3600.0));			// integrate the speed to get miles
	meta.feet = (float) (meta.miles*5280.0);
	meters = (float)(MILESTOMETERS * meta.miles);

	wattseconds += (meta.watts*tdt);
	meta.calories = (float)( (wattseconds / 4186) / .26);		// see physics.cpp for formula derivation
	return; 
}


/*************************************************************************

*************************************************************************/

void computrainerDecoder::runkeys(void)  {

	if (keydownfunc)  {					// realtime
#ifdef _DEBUG
		if (meta.keydown != 0)  {
			bp = 5;
		}
#endif

		switch(meta.keydown)  {
			case KEY1:
				keydownfunc(object, id, 1);
				#ifdef LOGKEYS
				//if (logg) logg->write("\nkey1 down\n");
				#endif
				break;

			case KEY2:
				keydownfunc(object, id, 2);
				#ifdef LOGKEYS
				//if (logg) logg->write("\nkey2 down\n");
				#endif
				break;

			case KEY3:
				keydownfunc(object, id, 3);
				#ifdef LOGKEYS
				//if (logg) logg->write("\nkey3 down\n");
				#endif
				break;

			case KEY4:
				// this can delete us!!!! Because scrollingChart handlePause() calls
				// ds->reset() [handlebarRTD::reset()] calls handlebarRTD::init() which DELETES us!
				keydownfunc(object, id, 4);
				#ifdef LOGKEYS
				//if (logg) logg->write("\nkey4 down\n");
				#endif
				break;

			case KEY5:
				keydownfunc(object, id, 5);
				#ifdef LOGKEYS
				//if (logg) logg->write("\nkey5 down\n");
				#endif
				break;

			case KEY6:
				keydownfunc(object, id, 6);
				#ifdef LOGKEYS
				//if (logg) logg->write("\nkey6 down\n");
				#endif
				break;

			case 0:														// call to allow key repeats (see ssv.cpp for example)
				keydownfunc(object, id, 0);
				break;

			default:									// multiple keys
				//if ( (meta.keydown & KEY3) && (meta.keydown & KEY6) )  {
				keydownfunc(object, id, meta.keydown);
				//}
				#ifdef LOGKEYS
				//if (logg) logg->write("\nkey6 down\n");
				#endif
				break;
		}
		meta.keydown = 0;
	}										// if (keydownfunc)  {

	if (keyupfunc)  {
		switch(meta.keyup)  {
			case KEY1:
				keyupfunc(object, id, 1);
				#ifdef LOGKEYS
				//if (logg) logg->write("key1 up\n");
				#endif
				break;

			case KEY2:
				keyupfunc(object, id, 2);
				#ifdef LOGKEYS
				//if (logg) logg->write("key2 up\n");
				#endif
				break;

			case KEY3:
				keyupfunc(object, id, 3);
				#ifdef LOGKEYS
				//if (logg) logg->write("key3 up\n");
				#endif
				break;

			case KEY4:
				keyupfunc(object, id, 4);
				#ifdef LOGKEYS
				//if (logg) logg->write("key4 up\n");
				#endif
				break;

			case KEY5:
				keyupfunc(object, id, 5);
				#ifdef LOGKEYS
				//if (logg) logg->write("key5 up\n");
				#endif
				break;

			case KEY6:
				keyupfunc(object, id, 6);
				#ifdef LOGKEYS
				//if (logg) logg->write("key6 up\n");
				#endif
				break;
					
			case 0:														// call to allow key repeats (see ssv.cpp for example)
				keyupfunc(object, id, 0);
				break;

			default:
				break;
		}
		meta.keyup = 0;
	}

	return;
}

/**********************************************************************************

**********************************************************************************/

void computrainerDecoder::set_watts_factor(float _f)  { 
	Decoder::set_watts_factor(_f);
	drag_aerodynamic = 8.0f * rd.watts_factor;			// max drag_aerodynamic is 4095 / 256 = 15.996 lbs
	// max watts_factor = 2.0
	return;
}


