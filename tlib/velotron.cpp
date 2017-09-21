
/**********************************************************************

**********************************************************************/

#ifdef WIN32
	#pragma warning (disable: 4786 4996)		// std library warnings
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <mmsystem.h>
#endif

#include <assert.h>

#include <tdefs.h>


//#include <fileselector.h>

#ifdef WIN32
	#include <fasttimer.h>
	#include <err.h>
#endif

#include <velotron.h>
#include <course.h>
#include <velotron_decoder.h>
#include <tglobs.h>

extern char gstring[2048];

/**********************************************************************

**********************************************************************/

Velotron::Velotron(const char *_port,  RIDER_DATA &_rd, Course *_course, Bike *_bike, LoggingType _logging_type) : RTD( _port, 38400, _rd, _course, _bike, false, _logging_type, "", -1 )   {

	if (gblog) log->write(10,0,1,"Velotron constructor, initialized = %s\n", initialized?"true":"false");

	if (!initialized)  {
		return;
	}

	init();

	if (gblog) log->write(10,0,1,"Velotron constructor exit, initialized = %s\n", initialized?"true":"false");
	if (!initialized)  {
		return;
	}

#ifdef LOG_MPH
	logstream = fopen("mph.txt", "wt");
#endif

}


/**********************************************************************
	destructor
**********************************************************************/

Velotron::~Velotron()  {

#ifdef LOG_MPH
	FCLOSE(logstream);
#endif

	destroy();
}



/******************************************************************************
	called as fast as possibe.
	returns 0 if there is new data
	if no data, returns 1 and no further processing is done.
******************************************************************************/

int Velotron::getNextRecord(void)  {

	connection_changed = false;

	if (!packetsAvailable())  {
		if (rxinptr == rxoutptr)  {
			if (connected)  {
				if ((timeGetTime() - lastCommTime) > timeout)  {
					if (last_connected)  {
						connection_changed = true;
					}
					connected = FALSE;
					last_connected = connected;
				}
			}
			return 1;
		}
	}


	if (!last_connected)  {
		last_connected = true;
		connection_changed = true;
	}

	connected = TRUE;
	lastCommTime = timeGetTime();


	decoder->decode(&rxq[rxoutptr*PACKETLEN]);

#ifdef LOG_MPH
	fprintf(logstream, "%.2f\n", decoder->meta.mph);
#endif

	switch(logging_type)  {
		case RAW_PERF_LOGGING:  {
			unsigned long now;

			if (decoder->get_started())  {
				if (!decoder->get_finished())  {
					if (!decoder->get_paused())  {
						if (outstream)  {
							if (recording)  {			// able to pause / continue recording
#ifdef _DEBUG
								//at->update();			// 5.00 ms, called from EV::threadProc()
#endif
								now = timeGetTime();
								fwrite(&now, sizeof(unsigned long), 1, outstream);
								fwrite(&rxq[rxoutptr*PACKETLEN], 1, PACKETLEN, outstream);
							}
#ifdef _DEBUG
							else  {
								bp = 2;
							}
#endif
						}
					}
				}
			}
			break;
		}

		case THREE_D_PERF_LOGGING:
			if (decoder->get_started())  {
				if (!decoder->get_finished())  {
					if (!decoder->get_paused())  {
						logtime += 5;			// assume 5 ms
						if (logtime==LOGTIME)  {
							logtime = 0;
							if (outstream) {
								makelogpacket();
								recordsOut++;
							}
						}
					}
				}
			}
			break;
		default:
			break;
	}



	if (decoder->get_finishEdge())  {
		finish();
	}

	rxoutptr++;
	if (rxoutptr==RXSIZE)  {
		rxoutptr = 0;
	}

	return 0;
}							// getNextRecord()


/******************************************************************************
	called as fast as possible.
	gets all available raw packets in the rxq.
	returns true if new packets are avail
******************************************************************************/

BOOL Velotron::packetsAvailable(void)  {
	int i, j, n;
	BOOL flag = FALSE;
	unsigned char chksum;

	assert(port);

	while(1)  {
		n = port->rx((char *)workbuf, CLEN);

		if (n==0)  {
			break;
		}

		for(i=0; i<n; i++)  {
			if (workbuf[i] & 0x80)  {
				if (packetIndex != 0)  {
					syncErrors++;
				}
				packetIndex = 0;
			}
			packet[packetIndex] = workbuf[i];
			packetIndex++;

			if (packetIndex==PACKETLEN)  {
				packetIndex = 0;
				chksum = 0;
				for(j=0;j<PACKETLEN-1;j++)  {
					packet[j] ^= code[j];							// decrypt the packet
					chksum += packet[j];
				}
				packet[9] ^= code[9];								// decrypt the checksum
				chksum &= 0x7f;


				if (packet[9] != chksum)  {
					checksumErrors++;
				}
				else  {							// good packet
					memcpy(&rxq[PACKETLEN*rxinptr], packet, PACKETLEN);		// push the packet on the queue
					rxinptr++;
					if (rxinptr==RXSIZE)  {
						rxinptr = 0;
					}
				}
			}
		}
	}

	return flag;

}			// packetsAvailable()

/**********************************************************************

**********************************************************************/

int Velotron::updateHardware(bool _force)  {
	DWORD now = timeGetTime();

	if (_force)   {
		lastPicCurrentCount= physics->currentCountToPic;
		b[0] = 0x80;
		b[1] = (physics->currentCountToPic & 0x03ff)>>4;		// bits 4-9  (0000 10uu uuuu 0000)
		b[2] = physics->currentCountToPic & 0x000f;			// bits 0-3  (0000 0000 0000 1111)

		port->txx(b, 3);

	}
	else if ((now - lastHWTime) >= UPDATE_MS )   {					// 47 ms
		lastHWTime = now;
		if (physics->currentCountToPic != lastPicCurrentCount)  {
			lastPicCurrentCount= physics->currentCountToPic;
			b[0] = 0x80;
			b[1] = (physics->currentCountToPic & 0x03ff)>>4;		// bits 4-9  (0000 10uu uuuu 0000)
			b[2] = physics->currentCountToPic & 0x000f;			// bits 0-3  (0000 0000 0000 1111)
		}
		port->txx((unsigned char *)b, 3);
	}

	return 0;

}

/**********************************************************************

**********************************************************************/

int Velotron::getPreviousRecord(DWORD _delay)  {		// dataSource pure virtual
	return getNextRecord();
}

/**********************************************************************

**********************************************************************/

//void Velotron::startCal(unsigned short picCurrentCount)  {
void Velotron::startCal(void)  {

	//calflag = true;

#if 1
	unsigned char buf[3];

	//load = ini->getInt("options", "load", "682", true);
	//xxx
	port->flush_tx();
	//port->flush();					// calls Serial::FlushBuffers()
	//port->flush_rawstream();
	//port->FlushBuffers();				// private

	unsigned short picCurrentCount = 667;

	buf[0] = 0x81;
	buf[1] = (picCurrentCount & 0x03ff)>>4;				// bits 4-9  (0000 10uu uuuu 0000)
	buf[2] = picCurrentCount & 0x000f;						// bits 0-3  (0000 0000 0000 1111)

	port->txx((unsigned char *)buf, 3);
#endif

	bp = 1;
	return;

}


/**********************************************************************

**********************************************************************/

unsigned char Velotron::start_cal(void)  {
	if (bike==NULL || physics==NULL)  {
		return 0xff;
	}

	front_index_save = bike->get_front_index();			// 2
	rear_index_save = bike->get_rear_index();			// 9
	bike->set_gear(93.0);
	physics->calibrating = true;
	setConstantCurrent(0);
	setPicCurrent(0);				// independent of physics
	//mode = (MODE)0;

	/*

												_f			_fs

		mode									0			PHYSICS_CONSTANT_CURRENT_MODE (3)
		physics->rpmLookupFactor				1.0			1.0
		physics->calibrating					true		true

		physics->bike->state.gear_inches		93.0		93.0					physics->bike->state.gear_inches
		physics->bike->state.use_default_gears	true		-----

		physics->bike->virtualFrontGear			56			56						physics->bike.gp.front
		physics->bike->virtualRearGear			23			11						physics->bike.gp.rear
		physics->bike->gear_inches_ix			---			29
		physics->bike->state.ngears				---			30
		physics->bike->override_gear_tables		---			true

	*/

	return 0;
}

/**********************************************************************

**********************************************************************/

void Velotron::end_cal(unsigned char last_control_byte)  {
	bike->set_gear(front_index_save, rear_index_save);
	physics->calibrating = false;
	return;
}

/**********************************************************************

**********************************************************************/

void Velotron::emergencyStop(int flag)  {
	physics->stopFlag = flag;
	return;
}

/**********************************************************************

**********************************************************************/

void Velotron::gradeUp(void)  {
	physics->gradeUp();
	decoder->meta.grade = (float)physics->grade;
	return;
}

/**********************************************************************

**********************************************************************/

void Velotron::gradeDown(void)  {
	physics->gradeDown();
	decoder->meta.grade = (float)physics->grade;
	return;
}

/**********************************************************************

**********************************************************************/

void Velotron::setGrade(double _grade)  {
	physics->grade = _grade;
	return;
}

/**********************************************************************

**********************************************************************/

void Velotron::set_draft_wind(float mph)  {
	//float mph = (float) (KPHTOMPH * _kph);

	dataSource::set_draft_wind(mph);
	physics->draftwind = mph;
	return;
}

/**********************************************************************

**********************************************************************/

void Velotron::set_wind(float mph)  {
	//float mph = (float) (KPHTOMPH * _kph);

	dataSource::set_x_wind(mph);
	physics->wind = mph;
	return;
}

/**********************************************************************

**********************************************************************/

void Velotron::destroy(void)  {
	FCLOSE(outstream);

	DEL(hr);

	if (rxq)  {
		delete[] rxq;
		rxq = NULL;
	}

	DEL(physics);

	DEL(rpmFilter);
	DEL(mphFilter);
	DEL(polarFilter);

	DEL(earLobeSSM);
	DEL(polarSSM);

	DEL(decoder);

}

/**********************************************************************

**********************************************************************/

void Velotron::pause(void)  {
	physics->pause();
	decoder->set_paused(true);
	return;
}

/**********************************************************************

**********************************************************************/

void Velotron::resume(void)  {
	physics->resume();
	decoder->set_paused(false);
	return;
}

/**********************************************************************

**********************************************************************/

void Velotron::setmiles(double _miles)  {
	decoder->meta.miles = (float)_miles;
	physics->setmiles(_miles);
	return;
}

/**********************************************************************

**********************************************************************/

void Velotron::set_start_miles(double _start_miles)  {
	decoder->set_startMiles((float)_start_miles);
	return;
}

/**********************************************************************
	used when the 'start' button is hit in the parent app
**********************************************************************/

void Velotron::reset(void)  {

	decoder->reset();
	decoder->meta.gear = (unsigned char) (.5 + bike->state.gear_inches);
	decoder->meta.virtualFrontGear = (unsigned char)bike->state.gp.front;
	decoder->meta.virtualRearGear = (unsigned char)bike->state.gp.rear;

	physics->resetIntegrators();
	physics->pause();									// tlm20040625

	RTD::reset();

#ifdef PERF3D
	gears.clear();
#endif

	decoder->ss->reset();					// ss is owned by Decoder:: and created in its constructor
	decoder->meta.minhr = (unsigned short) (.5 + rd.lower_hr);
	decoder->meta.maxhr = (unsigned short) (.5 + rd.upper_hr);

	return;
}

/**********************************************************************
	used by calibrator to control the current in the velotron
**********************************************************************/

void Velotron::setPicCurrent(unsigned short _currentCountToPic)  {
	static unsigned short lastPicCurrentCount=0xffff;
	DWORD now = timeGetTime();

	if ((now - lastHWTime) >= UPDATE_MS)  {
		lastHWTime = now;

		if (_currentCountToPic != lastPicCurrentCount)  {
			lastPicCurrentCount= _currentCountToPic;
			b[0] = 0x80;
			b[1] = (_currentCountToPic & 0x03ff)>>4;		// bits 4-9  (0000 10uu uuuu 0000)
			b[2] = _currentCountToPic & 0x000f;			// bits 0-3  (0000 0000 0000 1111)
		}

		port->txx((unsigned char *)b, 3);
	}


	return;
}

/**********************************************************************
	called after reset()
**********************************************************************/

void Velotron::start(void)  {
	decoder->set_started(true);
	decoder->set_paused(false);

	physics->ms = 0xffffffff-4;
	logtime = LOGTIME - 5;
	flush();									// get rid of backed up data

	decoder->packets = 0;						// tlm20040708		causes decoder timer to reset
	physics->set_paused(false);
	return;
}

/**********************************************************************

**********************************************************************/

void Velotron::init(void)  {
	//int itemp;

	if (gblog) log->write(10,0,1,"init()\n");

	front_index_save = 0;
	rear_index_save = 0;
	polarsig = 0.0f;
	memset(&lp, 0, sizeof(lp));
	//memset(dataFileName, 0, sizeof(dataFileName));
	timeout = 5000;
	logtime = 0;

	rpmNotification = false;

	physics = NULL;

	version = 3;
	// version 1: initial version
	// version 2: added metric flag. This is the metric checkmark on the main menu.
	// version 3:	began logging raw data
	//					added initial conditions structure
	//					added initial physics state

	elStrength = 0.0;
	polarStrength = 0.0;

	LOGTIME = 30;			//ini->getInt("options", "logtime", "30", true);
	rpmFilter = new LPFilter(200);		// rpm filter
	mphFilter = new LPFilter(200);		// rpm filter
	polarFilter = new LPFilter(50);

	earLobeSSM = new signalStrengthMeter(200);
	polarSSM = new signalStrengthMeter(200);

	hr = new Heartrate();

int i;

	for(i=0;i<PACKETLEN;i++)  {
		packet[i] = 0;
	}

	code[0] = 0x4b;
	code[1] = 0x7f;
	code[2] = 0x1d;
	code[3] = 0x5e;
	code[4] = 0x3c;
	code[5] = 0x29;
	code[6] = 0x6a;
	code[7] = 0x79;
	code[8] = 0x5a;
	code[9] = 0x46;

	if (gblog) log->write(10,0,1,"init2b\n");
	rxq = new unsigned char[RXSIZE*PACKETLEN];				// 128*7
	for(i=0;i<RXSIZE*PACKETLEN;i++)  {
		rxq[i] = 0;
	}

	if (gblog) log->write(10,0,1,"init2c\n");
	rxinptr = 0;
	rxoutptr = 0;
	syncErrors = 0;
	checksumErrors = 0;

	for(i=0;i<3;i++) b[i] = 0;
	lastPicCurrentCount = 0xffff;
	if (gblog) log->write(10,0,1,"init2d\n");

	if (gblog) log->write(10,0,1,"init2e\n");
	physics = new Physics(bike, (double)(rd.pd.kgs*TOPOUNDS), (double)rd.watts_factor );

	if (gblog) log->write(10,0,1,"init3\n");

	physics->pause();

	if (gblog) log->write(10,0,1,"init4\n");
	decoder = new velotronDecoder(course, rd, bike, physics, id);
	decoder->weight = (float)(rd.pd.kgs * TOPOUNDS);

	decoder->set_paused(true);
	decoder->meta.runSwitch = 1;
	decoder->set_started(false);
	decoder->set_finished(false);
	decoder->set_paused(false);

	if (gblog) log->write(10,0,1,"init5\n");

if (decoder->ss==0)  {
	printf("no ss\n");
}
else  {
	decoder->ss->set_degperseg(360.0 / 72.0);
}

	bp = 1;
	if (gblog) log->write(10,0,1,"init6\n");

	decoder->meta.minhr = (unsigned short) (.5 + rd.lower_hr);
	decoder->meta.maxhr = (unsigned short) (.5 + rd.upper_hr);

	if (gblog) log->write(10,0,1,"initx\n");

	return;
}							// init(void)

/*************************************************************************

*************************************************************************/

void Velotron::setPhysicsMode(Physics::MODE _mode)  {
	physics->setMode(_mode);

	return;
}

/**********************************************************************

**********************************************************************/

void Velotron::setConstantCurrent(float _constantCurrent)  {
	physics->setConstantCurrent(_constantCurrent);
	return;
}

/*************************************************************************

*************************************************************************/

void Velotron::setConstantWatts(float _constantWatts)  {
	physics->setConstantWatts(_constantWatts);
	physics->wfilter->setfval((double)_constantWatts);
	physics->dwfilter->setfval((double)_constantWatts);
	return;
}

/*************************************************************************

*************************************************************************/

float Velotron::getConstantWatts(void)  {
	return physics->getConstantWatts();
}

/*************************************************************************

*************************************************************************/

double Velotron::getWatts(void)  {
	return physics->getConstantWatts();
}

/*************************************************************************

*************************************************************************/

double Velotron::getDisplayedWatts(void)  {
	return physics->getDisplayedWatts();
}

/*************************************************************************

*************************************************************************/

void Velotron::setConstantForce(double _constant_force)  {
	if (physics)  {
		physics->set_constant_force(_constant_force);
	}
	return;
}



/*************************************************************************

*************************************************************************/

void Velotron::flush(void)  {

#ifdef WIN32
	port->flush();
#endif

	return;
}

/*************************************************************************

*************************************************************************/

void Velotron::kill_keys(void)  {
	int i, k;

	for(i=0; i<RXSIZE; i++)  {
		k = i*PACKETLEN + 8;					// byte 8 is the keys
		rxq[k] &= ~0x7e;						// gets rid of the key bits
	}

	return;
}

/*************************************************************************

*************************************************************************/

int Velotron::get_keys(void)  {
	if (!connected)  {
		return 0x40;
	}

#ifdef _DEBUG
	assert(decoder->keys==decoder->meta.keys);
#endif

	int keys = decoder->keys;								// get the accumulated keys since last read
	decoder->keys = 0;
	return keys;
}

/*************************************************************************

*************************************************************************/

// xyzzy

int Velotron::get_accum_tdc(void)  {
	return decoder->accum_tdc;
}

/*************************************************************************

*************************************************************************/

int Velotron::get_accum_keys(void)  {
	if (!connected)  {
		return 0x40;
	}

	int accum_keys = decoder->accum_keys;								// get the accumulated keys since last read
	decoder->accum_keys = 0;
	return accum_keys;
}

/*************************************************************************

*************************************************************************/

Bike::GEARPAIR Velotron::get_gear_pair(void)  {
	return decoder->get_gear_pair();
}

/*************************************************************************

*************************************************************************/

Bike::GEARPAIR Velotron::get_gear_indices(void)  {
	return decoder->get_gear_indices();
}

/*************************************************************************

*************************************************************************/

void Velotron::set_hr_bounds(int LowBound, int HighBound, bool BeepEnabled)  {

	if (BeepEnabled)  {
		rd.lower_hr = (float)LowBound;
		rd.upper_hr = (float)HighBound;
		rd.hr_beep_enabled = true;
	}
	else  {
		rd.lower_hr = 0.0f;
		rd.upper_hr = 255.0f;
		rd.hr_beep_enabled = false;
	}

	decoder->set_lower_hr(rd.lower_hr);
	decoder->set_upper_hr(rd.upper_hr);

	return;
}

/*************************************************************************

*************************************************************************/

void Velotron::set_grade(float _grade)  {
	//mode = WINDLOAD;
	/*
	Physics::PHYSICS_WIND_LOAD_MODE,				// old WINDLOAD mode
	Physics::PHYSICS_CONSTANT_TORQUE_MODE,
	Physics::PHYSICS_CONSTANT_WATTS_MODE,			// old ERGO mode
	Physics::PHYSICS_CONSTANT_CURRENT_MODE,
	Physics::PHYSICS_NOMODE
	*/

	Physics::mode = Physics::WINDLOAD;
	decoder->meta.grade = _grade;
	physics->set_grade(_grade);						// calls physics->setMode(Physics::WIND_LOAD_MODE)
	return;
}

/*************************************************************************

*************************************************************************/

void Velotron::setkgs(float _person_kgs)  {
	rd.pd.kgs = _person_kgs;
	decoder->set_weight_kgs(_person_kgs);						// also set it in the decoder
	physics->set_weight_lbs(TOPOUNDS*_person_kgs);

	return;
}

/*************************************************************************

*************************************************************************/

int Velotron::set_constant_force(float _force)  {
	physics->set_constant_force(_force);	
	return 0;
}

/*************************************************************************

*************************************************************************/

Physics* Velotron::get_physics_x(void)  {
	return physics;
}

