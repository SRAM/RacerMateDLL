
#define WIN32_LEAN_AND_MEAN

#ifdef WIN32
	#include <windows.h>
	#include <mmsystem.h>
#endif

#include <assert.h>

#include <utils.h>
#include <fatalerror.h>
#include <computrainer_decoder.h>
#include <computrainer.h>
#include <globals.h>

#include <round.h>

#ifndef WIN32
	#include <aunt.h>				// DO_ANT IS JUST DEFINED FOR WINDOWS RIGHT NOW
#endif

#ifdef _DEBUG
	#include <tglobs.h>
	#include <err.h>
#endif

/**********************************************************************

**********************************************************************/

// calls RTD:: constructor 1
Computrainer::Computrainer(
							const char *_port,
						   RIDER_DATA &_rd, 
						   Course *_course, 
						   Bike *_bike,
						   LoggingType _logging_type,
							const char *_url,
							int _tcp_port
						    ) : RTD( 
											_port,
											2400,
											_rd,
											_course,
											_bike,
											false,								// bool _show_errors
											_logging_type,
											_url,
											_tcp_port
											)   {

#ifdef _DEBUG
	static int calls = 0;
	calls++;
	if (calls == 3)  {
		bp = 0;
	}
#endif
	constructor = 1;

#ifdef _DEBUG
	//ctstream = fopen("ct.txt", "wt");
	ctstream = NULL;
	gulps = 0;
#endif

	//OutputDebugString("ct1");			// xxzz

	if (outstream)  {
		SEC sec;
		memset(&sec, 0, sizeof(sec));
		sec.type = SEC_COMPUTRAINER_RAW_DATA;
		sec.structsize = sizeof (sec);
		sec.n = 0L;

#ifdef _DEBUG
		//unsigned long offs;
		//offs = ftell(outstream);
#endif

		fwrite(&sec, sizeof(sec), 1, outstream);

#ifdef _DEBUG
		//offs = ftell(outstream);
#endif
	}

	if (log) log->write(10,0,1,"computrainer: 1\n");
        /*
	if (!initialized)  {
		if (log) log->write(10,0,1,"computrainer: 1x\n");
		return;
	}
        */
        
	init();
	if (log) log->write(10,0,1,"computrainer: 2\n");

	if (!initialized)  {
		bp = 1;
		if (log) log->write(10,0,1,"computrainer: 2x\n");
		return;
	}
	if (log) log->write(10,0,1,"computrainer: x\n");

}							// constructor 1




/**********************************************************************
  Computrainer needs a user because of:
	user->data.lbs
	user->ftp
	user->data.lower_hr
	user->data.upper_hr
**********************************************************************/

// uses RTD:: constructor 2
Computrainer::Computrainer( Serial *port, RIDER_DATA &_rd, Course *_course, Bike *_bike, LoggingType _logging_type ) : RTD( port, _rd, _course, false, _bike, _logging_type )   {

	/*
	if (broadcasting)  {
		bp = 1;
	}
	*/


	constructor = 2;

#ifdef _DEBUG
	ctstream = NULL;
	gulps = 0;
#endif

	if (outstream)  {
		unsigned long type = SEC_COMPUTRAINER_RAW_DATA;
		unsigned long size = 0L;
		fwrite(&type, sizeof(unsigned long), 1, outstream);
		fwrite(&size, sizeof(unsigned long), 1, outstream);
	}

	if (log) log->write(10,0,1,"computrainer: 1\n");

	if (!initialized)  {
		if (log) log->write(10,0,1,"computrainer: rtd not initialized\n");
		return;
	}

	if (log) log->write(10,0,1,"computrainer: 2\n");
	init();
	if (log) log->write(10,0,1,"computrainer: 3\n");

	if (!initialized)  {
		if (log) log->write(10,0,1,"computrainer: not initialized\n");
		return;
	}
	if (log) log->write(10,0,1,"computrainer: x\n");

}							// constructor 2

/**********************************************************************
  Computrainer needs a user because of:
	user->data.lbs
	user->ftp
	user->data.lower_hr
	user->data.upper_hr
**********************************************************************/

#if 1
#ifdef DO_RTD_SERVER
Computrainer::Computrainer( RTDServer *_server, RIDER_DATA &_rd, Course *_course, Bike *_bike, LoggingType _logging_type ) : RTD( _server, _rd, _course, false, _bike, _logging_type )   {
#else
Computrainer::Computrainer(int _ix, Server *_server, RIDER_DATA &_rd, Course *_course, Bike *_bike, LoggingType _logging_type ) : RTD(_ix, _server, _rd, _course, false, _bike, _logging_type )   {
#endif

	/*
	if (broadcasting)  {
		bp = 1;
	}
	*/


	constructor = 3;
#ifdef _DEBUG
	ctstream = NULL;
	gulps = 0;
#endif

	if (outstream)  {
		unsigned long type = SEC_COMPUTRAINER_RAW_DATA;
		unsigned long size = 0L;
		fwrite(&type, sizeof(unsigned long), 1, outstream);
		fwrite(&size, sizeof(unsigned long), 1, outstream);
	}

	if (log) log->write(10,0,1,"computrainer: 1\n");

	if (!initialized)  {
		if (log) log->write(10,0,1,"computrainer: rtd not initialized\n");
		return;
	}

	if (log) log->write(10,0,1,"computrainer: 2\n");
	init();
	if (log) log->write(10,0,1,"computrainer: 3\n");

	if (!initialized)  {
		if (log) log->write(10,0,1,"computrainer: not initialized\n");
		return;
	}
	if (log) log->write(10,0,1,"computrainer: x\n");

}							// constructor

#endif

#if 0
/**********************************************************************
  Computrainer needs a user because of:
	user->data.lbs
	user->ftp
	user->data.lower_hr
	user->data.upper_hr
**********************************************************************/

Computrainer::Computrainer( Client *_client, RIDER_DATA &_rd, Course *_course, Bike *_bike, LoggingType _logging_type ) : RTD( _client, _rd, _course, false, _bike, _logging_type )   {

	/*
	if (broadcasting)  {
		bp = 1;
	}
	*/

	constructor = 4;
#ifdef _DEBUG
	ctstream = NULL;
	gulps = 0;
#endif

	if (outstream)  {
		unsigned long type = SEC_COMPUTRAINER_RAW_DATA;
		unsigned long size = 0L;
		fwrite(&type, sizeof(unsigned long), 1, outstream);
		fwrite(&size, sizeof(unsigned long), 1, outstream);
	}

	if (log) log->write(10,0,1,"computrainer: 1\n");

	if (!initialized)  {
		if (log) log->write(10,0,1,"computrainer: rtd not initialized\n");
		return;
	}

	if (log) log->write(10,0,1,"computrainer: 2\n");
	init();
	if (log) log->write(10,0,1,"computrainer: 3\n");

	if (!initialized)  {
		if (log) log->write(10,0,1,"computrainer: not initialized\n");
		return;
	}
	if (log) log->write(10,0,1,"computrainer: x\n");

}							// constructor
#endif						// #if 0


/**********************************************************************
	destructor
**********************************************************************/

Computrainer::~Computrainer()  {
	destroy();
}

/******************************************************************************
	returns 0 if there is new VALID (or WAIT) data
******************************************************************************/

int Computrainer::getNextRecord(void)  {
	int n;
	int i;
	bool flag = false;
	unsigned long now;
	int rc = 0;

#ifdef FLASHLIGHT
	//timeout.start(1000);
#endif

	if (port)  {
		n = port->rx((char *)workbuf, CLEN-2);
	}

	/*
	else if (server)  {
		n = server->rx(ix, (char *)workbuf, CLEN-2);
	}
	else if (client)  {
		n = client->rx((char *)workbuf, CLEN-2);											// CLEN = 1024
	}
	*/

	now = timeGetTime();

	if (n > 0)  {
#ifdef _DEBUG
		gulps++;
		if (gulps==2)  {
			bp = 1;
		}
#endif
		bytes_in += (unsigned long) n;

#ifdef FLASHLIGHT
		if (n>100)  {
			throw(fatalError(__FILE__, __LINE__));
		}
#endif

		for(i=0;i<n;i++)  {
			packet[packetIndex] = workbuf[i];
			packetIndex++;
			if (packetIndex > 15) {		// overwrite last char after 16 without
				packetIndex=15;			// b7 set
			}

			if (workbuf[i] & 0x80)  {
#ifdef WIN32
				if (port)  {
					port->rxdiff = (port->rxinptr - port->rxoutptr)  &  (RXQLEN - 1);
				}
				/*
				else if (server)  {
					//server->rxdiff = (server->rxinptr - server->rxoutptr)  &  (RXQLEN - 1);
				}
				else if (client)  {
					//client->rxdiff = (client->rxinptr - client->rxoutptr)  &  (RXQLEN - 1);
				}
				*/
#endif
				if (packetIndex==7)  {

					// check the sync byte parity bit

					unsigned char cc = packet[6];
					int cnt = 0;
					int j;

					for(j=0; j<8; j++)  {
						if (cc&0x01)  {
							cnt++;
						}
						cc >>= 1;
					}
					if ((cnt%2)==0)  {
						packet_error1++;
						#ifdef _DEBUG
							if (port)  {
								port->flush_rawstream();
							}
							/*
							else if (server)  {
								//server->flush_rawstream();
							}
							else if (client)  {
								client->flush_rawstream();
							}
							*/
						#endif
					}
					else  {
						flag = true;

						// convert the 7 realtime bytes to 6 packed bytes for the decoder

						unsigned char work, pkt[6];
						int j;
						work = (unsigned char) (packet[6] & 0x3f);
						for(j=0;j<6;j++)  {
							pkt[j] = (unsigned char) ((packet[j]<<1) | ((work>>(5-j))&0x01));
						}


						int status;

						#ifdef _DEBUG
							if (ctstream)  {
								fprintf(ctstream, "%10ld  %d / %lu   ", timeGetTime(), n, bytes_in );
								int jj;
								for(jj=0; jj<5; jj++)  {
									fprintf(ctstream, "%02x ", pkt[jj]);
								}
								fprintf(ctstream, "%02x\n", pkt[jj]);
							}


							status = decoder->decode(pkt);			// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< DECODE <<<<<
							if (status)  {
								if (port)  {
									port->flush_rawstream();			// packet_error4
								}
								/*
								else  if (client)  {
									client->flush_rawstream();			// packet_error4
								}
								else  if (server)  {
									server->flush_rawstream(ix);			// packet_error4
								}
								*/
							}
						#else
							decoder->decode(pkt);					// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< DECODE <<<<<
						#endif


						if ( (decoder->meta.rpmflags & RPM_VALID) != RPM_VALID )  {
							#ifdef DO_ANT
							if (asi)  {
								if (cad.decoding_data)  {
									decoder->set_cadence(cad.get_val());
								}
								else if (sc.decoding_data)  {
									decoder->set_speed_cadence( sc.get_wheel_rpm(), sc.get_speed(), sc.get_cadence());
								}
								else  {
									decoder->set_cadence(-1.0f);
								}
							}
							#endif
						}
#ifdef _DEBUG
						else  {
							bp = 2;
						}
#endif
						if ( (decoder->meta.hrflags&0x0c)==0x0c)  {
							// hr sensor unplugged, so use the ANT heartrate sensor
//	fprintf(logstream, "gnr ok\n");
							//if (ant_stick_initialized)  {
							#ifdef DO_ANT
							if (asi)  {
//	fprintf(logstream, "ant_initialized\n");
								//decoder->set_hr(hr.get_hr());
								decoder->set_hr(hr.get_val());
							}
							#endif
						}
//	fclose(logstream);

						if (decoder->get_finishEdge())  {
							bp = 0;
						}
						else  {
							if (decoder->get_started())  {
								if (!decoder->get_finished())  {
									if (!decoder->get_paused())  {
										if (logging_type==RM1_PERF_LOGGING)  {
#ifdef DO_RMP
											if (rmp)  {
												rmpdata.Distance = (float)(TOKM*decoder->meta.miles);		// float, distance in KM
												rmpdata.Watts = (UINT16)ROUND(decoder->meta.watts);			// UINT16
												rmpdata.Watts_Load = decoder->meta.load;					// float
												rmpdata.Speed = (float)(TOKPH*decoder->meta.mph);			// float
												rmpdata.Cadence = (BYTE)ROUND(decoder->meta.rpm);			// BYTE
												rmpdata.Grade = (INT16)ROUND(decoder->meta.grade);			// INT16
												rmpdata.Wind = (float)(TOKPH*decoder->meta.wind);			// float
												rmpdata.HeartRate = (BYTE)ROUND(decoder->meta.hr);			// BYTE
												rmpdata.Calories = (UINT16)ROUND(decoder->meta.calories);	// UINT16
												rmpdata.PulsePower = (BYTE)ROUND(decoder->meta.pp);			// BYTE
												rmpdata.DragFactor = (BYTE)ROUND(100.0f*rd.watts_factor);	// BYTE
												rmpdata.RawCalibrationValue = (INT16)decoder->meta.rfmeas;	// INT16
												//rmpdata.FrontGear = (INT16)0;								// INT16
												//rmpdata.RearGear = (INT16)0;								// INT16
												//rmpdata.GearInches = (INT16)0;							// INT16
												rmpdata.TSS = decoder->formula->tss;						// float
												rmpdata.IF = decoder->formula->IF;							// float
												rmpdata.NP = decoder->formula->np;							// float
												rmpdata.SS = (BYTE)ROUND(decoder->meta.ss);					// BYTE
												rmpdata.SSLeftATA = (BYTE)decoder->meta.lata;				// BYTE
												rmpdata.SSRightATA = (BYTE)decoder->meta.rata;				// BYTE
												rmpdata.SSRight = (BYTE)ROUND(decoder->meta.rss);			// BYTE
												rmpdata.SSLeft = (BYTE)ROUND(decoder->meta.lss);			// BYTE
												rmpdata.SSLeftSplit = (BYTE)ROUND(decoder->meta.lwatts);	// BYTE
												rmpdata.SSRightSplit = (BYTE)ROUND(decoder->meta.rwatts);	// BYTE
												for(int jj=0; jj<24; jj++)  {
													rmpdata.bars[jj] = (BYTE)decoder->meta.bars[jj];		// BYTE
												}
												rmpdata.m_flags1 = 0;										// UINT16
												rmpdata.m_flags2 = 0;										// UINT16

												status = rmp->write_record(&rmpdata);
												if (status)  {
													bp = 1;
												}
											}

											else  {
												throw(fatalError(__FILE__, __LINE__, "rmp == null"));
											}
#endif			// #ifdef DO_RMP
										}
										else  {
											if (outstream)  {
												switch(logging_type)  {
													case RAW_PERF_LOGGING:
														// 1.0 megs per hour, lp is 10 bytes
														lp.time = decoder->meta.time;
														memcpy(lp.buf, pkt, 6);
														fwrite(&lp, sizeof(lp), 1, outstream);
														recordsOut++;
														break;

													case THREE_D_PERF_LOGGING:
														// about 5 megs per hour, pp is 48 bytes, counting winds and calories
														pp.hr = (unsigned char) ROUND(decoder->meta.hr);
														pp.rpm = (unsigned char) ROUND(decoder->meta.rpm);
														pp.watts = (unsigned short) ROUND(decoder->meta.watts);
														pp.kph = (float)(TOKPH*decoder->meta.mph);
														pp.ms = decoder->meta.time;
														pp.pp_s_grade_t_100 = (short) ROUND(decoder->meta.grade*100.0f);
														pp.left_ata = (unsigned char)decoder->meta.lata;
														pp.right_ata = (unsigned char)decoder->meta.rata;
														pp.kilometers = (float) (MILESTOKM * decoder->meta.miles);
														pp.leftss = (unsigned char) (ROUND(decoder->meta.lss));
														pp.rightss = (unsigned char) (ROUND(decoder->meta.rss));
														pp.leftsplit = (unsigned char) (ROUND(decoder->meta.lwatts));
														pp.rightsplit = (unsigned char) (ROUND(decoder->meta.rwatts));
														for(int ii=0; ii<SPIN_SEGS; ii++)  {
															pp.spin[ii] = (unsigned char) (ROUND(decoder->meta.bars[ii]));
														}

														fwrite(&pp, sizeof(PerfPoint), 1, outstream);
														winds.push_back(decoder->meta.wind);
														calories.push_back(decoder->meta.calories);
														recordsOut++;
														break;

													case RM1_PERF_LOGGING:
														bp = 1;
														break;

													default:
														break;
												}
											}			// if (outstream)
										}				// if (logging_type...
									}					// if (!paused)
								}						// if (!finished)
							}							// if (started)
						}
					}							// sync byte parity check
				}							// if (packetIndex==7)  {

#ifdef _DEBUG
				else  {
					// got a sync byte with packetIndex != 7
					// burst of cc's happen here
					packet_error2++;
					#ifdef _DEBUG
						if (port)  {
							port->flush_rawstream();
						}
						/*
						else if (client)  {
							client->flush_rawstream();
						}
						else  if (server)  {
							server->flush_rawstream(ix);
						}
						*/
					#endif
				}
#endif

				packetIndex = 0;
			}				// if (workbuf[i] & 0x80)  {
		}					// for(i...

		if (hrbeep_enabled())  {
			//---------------------------------------
			// check the heartrate limits & beep
			//---------------------------------------

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

			unsigned char flags = (unsigned char) (decoder->meta.hrflags & 0x0f);

			if ( (flags & 0x0c) != 0x0c)  {				// installed?
				if ( (flags & 0x02) != 0x02)  {		// no errors?
					if (decoder->meta.hr >= rd.upper_hr)  {									// user is the same as decoder->user
						if ( (now - lastbeeptime) > 400)  {			// 2.5 hz
							lastbeeptime = now;
							beep();
						}
					}
					else if (decoder->meta.hr <= rd.lower_hr)  {									// user is the same as decoder->user
						if (decoder->meta.hr >= 10)  {					// don't want 0 hr to beep!
							if ( (now - lastbeeptime) > 1000)  {		// 1 hz
								lastbeeptime = now;
								beep();
							}
						}
					}
				}
				else  {
					#ifdef _DEBUG
						bp = 1;
					#endif
				}
			}
		}			// if (hrbeep_enabled)

	}
	else  {		// no data
		if (connected)  {
			if ((now - lastCommTime) > 5000)  {
				connected = false;
			}
		}
		rc = 1;					// no data
		goto gndx;
	}

	connected = true;
	lastCommTime = now;


	if (flag)  {
		rc = 0;
	}
	else  {
		rc = 1;					// no data
	}

gndx:

#ifdef _DEBUG
	//at->update();				// 7.55 ms
#endif

#ifdef FLASHLIGHT
	//timeout.stop();
#endif

	return rc;
}							// getNextRecord()

// port

/**********************************************************************

**********************************************************************/

void Computrainer::init(void)  {

	manualWattsStep = 1.0f;
	bp = 0;
	DEL(decoder);

#ifdef _DEBUG
	if (id==0)  {
		bp = 1;
	}
#endif
	if (log) log->write(10,0,1,"computrainer::init: 1\n");
	decoder = new computrainerDecoder(course, rd, id);


	memset(&lp, 0, sizeof(lp));
	memset(packet, 0, sizeof(packet));

	pkt_mask[0] = 0x60;
	pkt_mask[1] = 0x50;
	pkt_mask[2] = 0x48;
	pkt_mask[3] = 0x44;
	pkt_mask[4] = 0x42;
	pkt_mask[5] = 0x41;

	is_signed = 0;

	done_finished = false;

	rr = 0;
	control_byte = 0x00;
	manualWatts = 0.0f;
	skips = 0;
	lastWriteTime = 0;
	tx_select = 0;

	decoder->meta.runSwitch = 1;
	decoder->meta.brakeSwitch = 0;
	lastGrade_i = 0;
	lastWind_i = 0;

	control_byte = HB_WIND_RUN;
	if (log) log->write(10,0,1,"computrainer::init: x\n");

	return;
}						// init()


/**************************************************************************
Norm: this applies to the xx.33-xx.35 handlebar versions, the only ones we
ever shipped. Vers xx.39 with encryption is the only one I found that used
the aerodynamic drag stuff.

TRANSMITTED PACKET FORMAT:

	txbuf[0] ... [2] are unused in the handlebar for now

	txbuf[3] = x x x x x x x x = control flags
              | | | | | | | |
              | | | | | | |  --  unused, set to 0
              | | | | | |  ----  unused, set to 0
              | | | | |  ------  0x04 = make load, 0 = paused, unload user
              | | | |  --------	0x08 = mode control 0
              | | |  ----------  0x10 = mode control 1
              | |  ------------  0x20 = enable spinscan data, 0 = none sent
              |  --------------  transmit rate low bit (see below)
               ----------------  transmit rate high bit (see below)

		transmit rate bits:

			00 = 33.0 hz
			01 = 25 hz
			10 = 20 hz
			11 = 20 hz

		mode control bits:
			0x00 = old pts load level mode, load is set by id 0x70 data below
			0x08 = wind load mode, set by wind, weight, grade
			0x10 = ergometer mode, at tests, constant watts load set by 0x80 data
			0x18 = rolling friction calibrate mode

	control flag values: no data sent to hb sets standalone mode

Norm: mode control bits == 00 still select the "pts" load mode, same as
		standalone. The pts setting, id 0x70 ranges from 0 to 59.

		I left the aero and tire drag handicap outputs enabled, so this will
      work if we change the hb code to use them. drag_tire would be used
      for the standalone load level but we should never set this mode anyway.



//#define	HB_STANDALONE	0x00 dont use this mode.



#define	HB_WIND_RUN		0x2c
#define	HB_WIND_PAUSE	0x28
#define	HB_RFTEST		0x1c
#define	HB_ERGO_RUN		0x14
#define	HB_ERGO_PAUSE	0x10
	

	txbuf[4] = x x x x x x x x
              | | | | | | | |
              | | | | | | |  ------ bit  8 of data
              | | | | | |  -------- bit  9 of data
              | | | | |  ---------- bit 10 of data
              | | | |  ------------ bit 11 of data
              | | | |
              | | |  -------------- data id bit 0
              | |  ---------------- data id bit 1
              |  ------------------ data id bit 2
               -------------------- data id bit 3

	txbuf[5] = bits 0 - 7 of data


ID INFORMATION:
---------------

0x00		unused, ignored

0x10		grade, %*10, signed
0x20		wind, mph, signed
0x30		weight, lbs, unsigned
0x40		min hr, 0 = no alarms, else 40-254
0x50		max hr, 41-255, should be > min hr
0x60		"target hr", ignored; in v39 it's drag_aerodynamic
0x70		"pts/standalone" load, ignored if mode==00; in v39 is drag_tire
0x80		power level, in watts for ergometer mode

0x90		ignored...
0xa0
0xb0
0xc0
0xd0
0xe0
0xf0		request data, currently ignored, hb sends all it has

  note: this maintains is own 10hz timer

**********************************************************************/

int Computrainer::updateHardware(bool _force)  {			// dataSource:: pure virtual
	unsigned char work;

#ifdef _DEBUG
	int i=-1;
#else
	int i=-1;
#endif

	//if (port==NULL && server==NULL && client==NULL)  {
	if (port==NULL)  {
		return 0;
	}

	DWORD now;

	now = timeGetTime();
	if ( (now-lastWriteTime) < 100)  {
		return 0;
	}

#ifdef _DEBUG
	//at->update();		// 104.6 ms in windows, 100.000 ms in linux!
#endif

	lastWriteTime = now;

#ifdef WIN32
	if (port)  {
		port->txdiff = (port->txinptr - port->txoutptr)  &  (TXQLEN - 1);
	}
	/*
	else if (server)  {
		//server->txdiff = (server->txinptr - server->txoutptr)  &  (TXQLEN - 1);
	}
	else if (client)  {
		//server->txdiff = (client->txinptr - client->txoutptr)  &  (TXQLEN - 1);
	}
	*/
#endif

	tx_select = (unsigned char) ((tx_select & 0x70) + 0x10);		// 0x10 to 0x80



	switch(tx_select)  {

		case 0x10:								// grade	(-15.0 to +15.0)
			if (registered)  {
				i = (int) iround(decoder->meta.grade*10);							//decoder->meta.grade;

				if (decoder->meta.grade != last_export_grade)  {
					last_export_grade = decoder->meta.grade;
					if (export_stream)  {
						fprintf(export_stream, "%10ld %.3f\n", timeGetTime(), decoder->meta.grade);
						fflush(export_stream);
					}
				}
			}
			else  {
				i = 0;
			}
			is_signed = 1;
			break;

		case 0x20:  {						// wind
			is_signed = 1;
#ifdef _DEBUG
			if (decoder->meta.wind > 0.01f)  {
				bp = 1;
			}
#endif
			i = (int) iround(decoder->meta.wind + decoder->draft_wind);
			break;
		}

		case 0x30:
			is_signed = 0;
			i = (int) (.5 + TOPOUNDS*rd.pd.kgs);					// rd.pd.kgs defaults to 160 lbs.
#ifdef _DEBUG
			if (i != 150)  {
				//throw(fatalError(__FILE__, __LINE__));
			}
#endif
			break;

		case 0x40:
			is_signed = 0;
			i = (int) (.5 + rd.lower_hr);
			i |= 0x0800;							// set bit 11 for hb to understand
			break;

		case 0x50:
			is_signed = 0;
			i = (int) (.5 + rd.upper_hr);
			i |= 0x0800;							// set bit 11 for hb to understand
			break;

		case 0x60:  {
			is_signed = 0;

			i = (int) (.5 + 256.0*decoder->drag_aerodynamic);		// 8.0 is default, 4095 is max (15.99 lbs)

#ifdef _DEBUG
			static int lasti = 2048;
			/*
				 50		 .5
				100		1.0		 8.0	2048
				150		1.5		12.0	3072
				200		2.0		16.0	4096

			*/

			/*
			if (i != 2048)  {
				bp = 4;
			}

			if (i>4095)  {
				bp = 3;
			}
			*/

			if (i != lasti)  {
				lasti = i;
			}
			//i = 0;
#endif
			break;
		}
		case 0x70:  {
			is_signed = 0;
			double d;
			d = decoder->drag_tire*(TOPOUNDS*rd.pd.kgs);
			i = (int) (.5 + 256*d);
#ifdef _DEBUG
			/*
			if (i != 230)  {
				bp = 4;
			}
			*/
#endif
			break;
		}

		case 0x80:
			is_signed = 0;
			i = (int) (.5 + manualWatts);
			break;

		default:
			return 0;
	}

	if (is_signed)  {									// signed?
		if (i>2047)  {
			i = 2047;
		}
		if (i<-2048)  {
			i = -2048;
		}
	}
	else  {
		if (i<0)  {
			i = 0;
		}
		if (i>4095)  {
			i = 4095;
		}
	}

	/*
			enum COMPUTRAINER_MODE  {
				HB_WIND_RUN = 0x2c,
				HB_WIND_PAUSE = 0x28,
				HB_RFTEST = 0x1c,
				HB_ERGO_RUN = 0x14,
				HB_ERGO_PAUSE = 0x10
			};
	*/

#ifdef _DEBUG
	switch(control_byte)  {
		case HB_WIND_RUN:								// 0x2c
			bp = 1;
			break;
		case HB_WIND_PAUSE:								// 0x28
			bp = 1;
			break;
		case HB_RFTEST:									// 0x1c
			bp = 1;
			break;
		case HB_ERGO_RUN:								// 0x14
			bp = 1;
			break;
		case HB_ERGO_PAUSE:								// 0x10
			bp = 2;
			break;
		default:
			bp = 7;
			break;
	}
#endif

	txbuf[4] = (unsigned char) (tx_select | (((unsigned char) (i>>8)) & 0x0f));
	txbuf[5] = (unsigned char) (i&0xff);
	txbuf[3] = control_byte;

	txbuf[1] = 0;
	txbuf[2] = 0;


	work = txbuf[1];
	for(i=2;i<6;i++)  {
		work = (unsigned char)((work+txbuf[i]) & 0xff);
	}
	txbuf[0] = (unsigned char) (work ^ 0xff);


	work = 0x80;								// header byte


	unsigned char buf[7];


	for(i=0;i<6;i++)  {
		buf[i] = (char)(txbuf[i]>>1);
  		if (txbuf[i]&0x01)  {
			work ^= pkt_mask[i];
		}
	}
	buf[i] = work;

	bytes_out += 7;

#ifdef WIN32
	if (port)  {
		port->txx(buf, 7);
	}
	/*
	else  if (server)  {
		server->txx(buf, 7);
	}
	else  if (client)  {
		client->txx(buf, 7);
	}
	*/
#else
	port->tx((unsigned char *)buf, 7);
#endif
	
	return 0;

}															// updateHardware()

//port

/**********************************************************************

**********************************************************************/

int Computrainer::getPreviousRecord(DWORD _delay)  {		// dataSource pure virtual
	//return getNextRecord(0);
	return getNextRecord();
}


/**********************************************************************

**********************************************************************/

void Computrainer::pause(void)  {

	decoder->set_pause_start_time(timeGetTime());
	decoder->set_paused(true);

#ifdef _DEBUG
		decoder->log(10,0,1,"\npause:\n\tstartTime = %ld\n\tpauseStartTime = %ld\n\tpausedTime = %ld", 
			decoder->get_start_time(),
			decoder->get_pause_start_time(),
			decoder->get_paused_time()
			);
#endif

	if (control_byte==HB_ERGO_RUN)  {
		control_byte = HB_ERGO_PAUSE;
	}
	else  {
		control_byte = HB_WIND_PAUSE;
	}

	return;
}							// void Computrainer::pause(void)  {

/**********************************************************************

**********************************************************************/

void Computrainer::resume(void)  {
	DWORD dw;

	if (!decoder->get_paused())  {
		return;
	}

	dw = decoder->get_paused_time();
#ifdef _DEBUG
	if (dw != 0)  {
		bp = 7;
	}
#endif

	dw += ( timeGetTime() - decoder->get_pause_start_time() );
	decoder->set_paused_time(dw);


#ifdef _DEBUG
	decoder->log(10,0,1,"\npause:\n\tstartTime = %ld\n\tpauseStartTime = %ld\n\tpausedTime = %ld", 
		decoder->get_start_time(),
		decoder->get_pause_start_time(),
		decoder->get_paused_time()
		);
#endif

	decoder->set_paused(false);

	if (control_byte==HB_ERGO_PAUSE)  {
		control_byte = HB_ERGO_RUN;
	}
	else if (control_byte==HB_WIND_PAUSE)  {
		control_byte = HB_WIND_RUN;
	}
	else  {
		return;
	}

	return;
}

/**********************************************************************

**********************************************************************/

void Computrainer::start(void)  {
	decoder->packets = 0;						// tlm20040708		causes decoder timer to reset
	decoder->set_started(true);
	decoder->set_paused(false);

	
	SYSTEMTIME x;
	GetLocalTime(&x);
	sprintf(started_date_time,"%d-%02d-%02d %02d:%02d:%02d",x.wYear, x.wMonth, x.wDay, x.wHour, x.wMinute, x.wSecond);

	decoder->set_startMiles(decoder->meta.miles);

	if (decoder->ss)  {
		decoder->ss->start();
	}

	return;
}									// start()


/**********************************************************************

**********************************************************************/

void Computrainer::gradeUp(void)  {
	decoder->gradeUp();
	return;
}

/**********************************************************************

**********************************************************************/

void Computrainer::gradeDown(void)  {
	decoder->gradeDown();
	return;
}

/**********************************************************************

**********************************************************************/

double Computrainer::getGrade(void)  {
	return decoder->meta.grade;
}


/**********************************************************************

**********************************************************************/

void Computrainer::reset(void)  {

	RTD::reset();

	decoder->reset();		// to reset meta

	done_finished = false;
	lastGrade_i = 0;
	lastWind_i = 0;
	manualWatts = 0.0f;

	decoder->meta.minhr = (unsigned short) (.5 + rd.lower_hr);
	decoder->meta.maxhr = (unsigned short) (.5 + rd.upper_hr);

	decoder->meta.runSwitch = 1;
	decoder->meta.time = 0;

	return;
}								// reset()



/**********************************************************************

**********************************************************************/

void Computrainer::destroy(void)  {
	//unsigned long size, size2;
	//unsigned long offs;
	//SecType sectype;
	//int n;
	//SEC tsec;


#ifdef _DEBUG
	FCLOSE(ctstream);
#endif


	#ifdef MYDBG
	FCLOSE(dbgstream);
	#endif

#if 1
	if (outstream)  {
		fflush(outstream);
		FCLOSE(outstream);			// rtd:: will also close the file if it is open
	}
#else
	if (outstream)  {
		fflush(outstream);
		FCLOSE(outstream);			// rtd:: will also close the file if it is open

		size = filesize_from_name(outfilename);

		//-----------------------------------
		// rewrite the perf_file_header:
		//-----------------------------------

		outstream = fopen(outfilename, "r+b");
		fseek(outstream, 0L, SEEK_SET);

		perf_file_header.sec.n = recordsOut;
		perf_file_header.sec.structsize = sizeof(perf_file_header);
		fwrite(&perf_file_header, sizeof(perf_file_header), 1, outstream);
		fflush(outstream);

		size2 = sizeof(perf_file_header);
		offs = ftell(outstream);
		FCLOSE(outstream);

		size2 = filesize_from_name(outfilename);
		if (size2 != size)  {
			DEL(decoder);
			strcpy(gstring, "perf file size error");
			throw(fatalError(__FILE__, __LINE__, gstring));
		}

		//------------------------------------------
		// get a list of the section offsets
		//------------------------------------------

		outstream = fopen(outfilename, "rb");

		i = 0;

		while(1)  {
			tsec.struct_start_offset = ftell(outstream);
			n = fread(&sectype, sizeof(unsigned long), 1, outstream);
			if (n != 1)  {
				break;
			}
			n = fread(&size2, sizeof(unsigned long), 1, outstream);
			if (n != 1)  {
				break;
			}

			memset(&tsec, 0, sizeof(tsec));

			tsec.structsize = size2;
			tsec.type = sectype;
			secs.push_back(tsec);

			if (sectype==SEC_PERF_FILE_HEADER)  {
				bp = 1;
			}
			else if (sectype==SEC_RIDER_INFO)  {
				bp = 2;
			}
			else if (sectype==SEC_RESULT)  {
				bp = 2;
			}
			else if (sectype==SEC_COMPUTRAINER_RAW_DATA)  {
				size2 = perf_file_header .sec.n*sizeof(computrainerDecoder::RAW_COMPUTRAINER_LOGPACKET);
				secs[i].structsize = size2;
				status = fseek(outstream, size2, SEEK_CUR);
				if (status)  {
					break;
				}
				continue;
			}
			else  {
				bp = 3;
			}

			status = fseek(outstream, size2 - 8, SEEK_CUR);
			if (status)  {
				break;
			}
			i++;
		}

		FCLOSE(outstream);

#ifdef _DEBUG

		if (log) log->write("\nsections:\n");

		size2 = secs.size();
		int ix;

		for(i=0; i<(int)size2; i++)  {
			ix = (int)secs[i].type;
			if (log) log->write("%10ld  %10ld   %s\n", secs[i].struct_start_offset, secs[i].structsize, sec_type_str[ix]);
		}
#endif


		size2 = filesize_from_name(outfilename);
		if (size2 != size)  {
			DEL(decoder);
			strcpy(gstring, "perf file size error");
			throw(fatalError(__FILE__, __LINE__, gstring));
		}


		//-------------------------------------------
		// rewrite any sections that need to be:
		//-------------------------------------------

		//CRF *s = new CRF();

		size2 = secs.size();

		for(i=0; i<(int)size2; i++)  {
			tsec = secs[i];
			if (tsec.type==SEC_PERF_FILE_HEADER)  {
				bp = 7;
			}
			else if (tsec.type==SEC_RIDER_INFO)  {
				// re-write the rider info:
				outstream = fopen(outfilename, "r+b");
				fseek(outstream, tsec.struct_start_offset, SEEK_SET);
				//s->init();
				//s->doo(rinf.
				fwrite(&rinf, sizeof(rinf), 1, outstream);
				FCLOSE(outstream);
			}
			else if (tsec.type==SEC_COMPUTRAINER_RAW_DATA)  {
				outstream = fopen(outfilename, "r+b");
				fseek(outstream, tsec.struct_start_offset, SEEK_SET);
				fwrite(&perf_file_header, sizeof(perf_file_header), 1, outstream);
				FCLOSE(outstream);

				bp = 3;
			}
			else if (tsec.type==SEC_RESULT)  {
				// re-write the results
				bp = 7;
			}
			else  {
				bp = i;
			}
		}

		//DEL(s);

		bp = 1;

		size2 = filesize_from_name(outfilename);

		if (size2 != size)  {
			DEL(decoder);
			strcpy(gstring, "perf file size error");
			throw(fatalError(__FILE__, __LINE__, gstring));
		}

		//----------------------
		// add the course:
		//----------------------

		if (course)  {
#ifdef _DEBUG
			long sz1 = filesize_from_name(outfilename);
#endif
			outstream = fopen(outfilename, "a+b");
			tsec.struct_start_offset = ftell(outstream);
			fseek(outstream, 0L, SEEK_END);
			tsec.struct_start_offset = ftell(outstream);
#ifdef _DEBUG
			if (sz1 != tsec.struct_start_offset)  {
				throw(fatalError(__FILE__, __LINE__));
			}
#endif

			course->write(outfilename, outstream);
			FCLOSE(outstream);

#ifdef _DEBUG
			int npoints;
			npoints = course->get_npoints();

			sz1 = filesize_from_name(outfilename);
			long diff = sz1 - tsec.struct_start_offset;
			if (diff != course->get_npoints()*sizeof(FPOINT) + sizeof(COURSE_HEADER))  {
				throw(fatalError(__FILE__, __LINE__));
			}
#endif
			tsec.type = SEC_COURSE;
			tsec.structsize = sizeof(COURSE_HEADER) + course->get_npoints()*sizeof(FPOINT);
			secs.push_back(tsec);
#ifdef _DEBUG
			if (constructor == 1)  {
				dump_perf(outfilename);
			}
#endif
		}
	}

#endif								// #if 0

#ifdef _DEBUG
	if (decoder->get_started())  {
		if (!decoder->get_finished())  {
			bp = 7;
		}
	}
#endif

	DEL(decoder);


	return;
}								// destroy

#ifdef WIN32

/*************************************************************************

*************************************************************************/

char * Computrainer::save(bool lastperf, bool _showDialog)  {
	char fname[256];
	char curdir[256];

	if (saved)  {
		return 0;
	}

	GetCurrentDirectory(255, curdir);

	memset(fname, 0, sizeof(fname));
	strcpy(export_name, fname);
	return export_name;
}					// save()

#endif			// win32

/*************************************************************************

*************************************************************************/

void Computrainer::dorr(void)  {

	if (decoder->get_started())  {
		if (!decoder->get_paused())  {
			if (!rr)  {
				// if started and not paused and not in calibration mode, ignore
				return;
			}
		}
	}

	rr ^= 1;

	if (rr)  {
		control_byte = HB_RFTEST;			// HB_WIND_PAUSE;
	}
	else  {
		control_byte = HB_WIND_RUN;
	}

	return;
}

#ifdef WIN32
/*************************************************************************
	decoder has set the finishedge flag
*************************************************************************/

void Computrainer::finish(void)  {
	decoder->set_finished(true);
	decoder->set_finishEdge(true);
	FCLOSE(outstream);
	return;
}
#endif

/*************************************************************************

*************************************************************************/

void Computrainer::flush(void)  {

	packetIndex = 0;
	memset(packet, 0, sizeof(packet));

	if (port==NULL)  {
		return;
	}
	port->flush();
	return;
}

/*************************************************************************

*************************************************************************/

void Computrainer::setkgs(float _person_kgs)  {
	rd.pd.kgs = _person_kgs;
	decoder->set_weight_kgs(_person_kgs);						// also set it in the decoder
	
	return;
}


/*************************************************************************

*************************************************************************/
/*
void Computrainer::set_drag_factor(float _drag_factor)  {
	decoder->set_watts_factor(_drag_factor);
	rd.watts_factor = _drag_factor;
	return;
}
*/

/*************************************************************************

*************************************************************************/

void Computrainer::set_hr_bounds(int LowBound, int HighBound, bool BeepEnabled)  {

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

int Computrainer::get_keys(void)  {
	if (!connected)  {
		return 0x40;
	}

#ifdef _DEBUG
	if (decoder->keys!=decoder->meta.keys)  {
		bp = 1;
	}
	//assert(decoder->keys==decoder->meta.keys);
#endif

	int keys = decoder->keys;								// get the accumulated keys since last read
	decoder->keys = 0;
	return keys;
}

/*************************************************************************

*************************************************************************/

int Computrainer::get_accum_tdc(void)  {
	return decoder->accum_tdc;
}

/*************************************************************************

*************************************************************************/

int Computrainer::get_status_bits(void)  {
	return decoder->hbstatus;
}

/*************************************************************************

*************************************************************************/

int Computrainer::get_accum_keys(void)  {
	if (!connected)  {
		return 0x40;
	}

	int accum_keys = decoder->accum_keys;								// get the accumulated keys since last read
	
	//decoder->accum_keys = 0;
	decoder->accum_keys = decoder->keys;

	return accum_keys;
}

/*************************************************************************

*************************************************************************/

unsigned char Computrainer::start_cal(void) {
	unsigned char last_control_byte = control_byte;
	/*
	enum COMPUTRAINER_MODE  {
		HB_WIND_RUN = 0x2c,
		HB_WIND_PAUSE = 0x28,
		HB_RFTEST = 0x1c,
		HB_ERGO_RUN = 0x14,
		HB_ERGO_PAUSE = 0x10
	};
	*/
	control_byte = HB_RFTEST;				// sent by updateHardware()
	updateHardware();

	return last_control_byte;
}

/*************************************************************************

*************************************************************************/

/*
enum COMPUTRAINER_MODE  {
	HB_WIND_RUN = 0x2c,
	HB_WIND_PAUSE = 0x28,
	HB_RFTEST = 0x1c,
	HB_ERGO_RUN = 0x14,
	HB_ERGO_PAUSE = 0x10
};
*/

void Computrainer::end_cal(unsigned char last_control_byte)  {
	control_byte = last_control_byte;
	return;
}

/*************************************************************************

*************************************************************************/

void Computrainer::setConstantWatts(float _watts)  {
	control_byte = HB_ERGO_RUN;
	//mode = ERGO;
	/*
		enum PHYSICS_MODE  {
			PHYSICS_WIND_LOAD_MODE,					// old WINDLOAD mode
			PHYSICS_CONSTANT_TORQUE_MODE,
			PHYSICS_CONSTANT_WATTS_MODE,			// old ERGO mode
			PHYSICS_CONSTANT_CURRENT_MODE,
			PHYSICS_NOMODE
		};
	*/

	Physics::mode = Physics::CONSTANT_WATTS;

	manualWatts = _watts;
	return;
}

/*************************************************************************

*************************************************************************/

void Computrainer::set_grade(float _grade)  {
	control_byte = HB_WIND_RUN;
	
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
	return;
}

/*************************************************************************
	makes a 3d compatible log packet
*************************************************************************/

//void Computrainer::makelogpacket(void)  {
	//lp.time = timeGetTime();
//	return;
//}


/*************************************************************************

*************************************************************************/

RESULT Computrainer::get_result(void)  {

	if (!decoder->get_started())  {
		memset(&result, 0, sizeof(RESULT));
		return result;
	}


/*
typedef struct  {
	int id;
	char edate[20];
	char lname[40];
	char fname[40];
	char gender;
	float age;
	float kgs;
	float bike_kgs;
	char email[128];
	char city[40];
	char state[20];
	char country[32];
	char zip[20];
	unsigned long ms;								// elapsed ms
	float average_kph;
	float max_kph;
	float average_watts;
	float max_watts;
	float average_wpkg;
	float average_ss;
	float max_ss;
} RESULT;
*/

	strncpy(result.lname, rd.pd.lastname.c_str(), sizeof(result.lname)-1);
	strncpy(result.fname, rd.pd.firstname.c_str(), sizeof(result.fname)-1);
	strncpy(result.username, rd.pd.username.c_str(), sizeof(result.username)-1);
	result.gender = rd.pd.sex;
	result.age = (float)rd.pd.age;
	result.year = rd.pd.year;
	result.month = rd.pd.month;
	result.day = rd.pd.day;
	result.cal = rd.cal;
	//strncpy(result.site, rd.site.c_str(), sizeof(result.site)-1);

	result.kgs = (float)rd.pd.kgs;
	result.test_kgs = (float)rd.pd.kgs;
	result.bike_kgs = 10.0f;
	strncpy(result.email, rd.pd.email.c_str(), sizeof(result.email)-1);
	strncpy(result.test_city, rd.pd.city.c_str(), sizeof(result.test_city)-1);
	strncpy(result.test_state, rd.pd.state.c_str(), sizeof(result.test_state)-1);
	strncpy(result.test_country, rd.pd.country.c_str(), sizeof(result.test_country)-1);
	strncpy(result.test_zip, rd.pd.zip.c_str(), sizeof(result.test_zip)-1);

	result.ms = decoder->meta.time;
	result.average_kph = (float) (TOKPH*decoder->meta.averageMPH);
	result.max_kph = (float) (TOKPH*decoder->meta.peakMPH);
	result.average_watts = decoder->meta.averageWatts;
	result.max_watts = decoder->meta.peakWatts;
	result.average_wpkg = (float) (decoder->meta.averageWatts / rd.pd.kgs);
	result.average_ss = decoder->meta.average_ss;
	result.max_ss = decoder->get_max_ss();
	result.average_rpm = decoder->meta.averageRPM;
	result.max_rpm = decoder->meta.peakRPM;

	strncpy(result.edate, started_date_time, sizeof(result.edate));				// the time the race started
	/*
	SYSTEMTIME x;
	GetLocalTime(&x);
	sprintf(result.edate,"%d-%02d-%02d %02d:%02d:%02d",x.wYear, x.wMonth, x.wDay, x.wHour, x.wMinute, x.wSecond);		// now
	*/

	result.cal = 100*decoder->meta.rfmeas;			//get_calibration();
//	result.cal = decoder->meta.rfdrag;
//	result.cal = get_calibration();

	if (!decoder->get_finished())  {
		return result;
	}

	// fill in the results

	return result;
}

/*************************************************************************

*************************************************************************/
/*
const char *Computrainer::get_mode_name(unsigned char i)  {
	const char *cptr;
	switch(i)  {
		case 0x2c:
			cptr = "HB_WIND_RUN";
			break;
		case 0x28:
			cptr = "HB_WIND_PAUSE":
			break;
		case 0x1c:
			cptr = "HB_RFTEST";
			break;
		case 0x14:
			cptr = "HB_ERGO_RUN";
			break;
		case 0x10:
			cptr = "HB_ERGO_PAUSE";
			break;
		default:
			cptr = "HB_UNKNOWN";
			break;
	}
	return cptr;
}
*/

