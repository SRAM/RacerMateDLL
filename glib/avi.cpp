#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <utils.h>

#include <avi.h>

#pragma warning(disable:4996)					// for vista strncpy_s

/**********************************************************************************************************
	constructor
	http://www.mikekohn.net

**********************************************************************************************************/

AVI::AVI(const char *_fname)  {

	strncpy(fname, _fname, sizeof(fname));
	init();

	return;
}

/**********************************************************************************************************
	destructor
**********************************************************************************************************/

AVI::~AVI(void)  {

	DEL(logg);
	return;
}

/**********************************************************************************************************

**********************************************************************************************************/

int AVI::init(void)  {

	bp = 0;
	logg = NULL;

	stream = fopen(fname,"rb");
	if (stream==0) {
		return 1;
	}

	parse_riff(stream);

	fclose(stream);

	return 0;
}


/**********************************************************************************************************

**********************************************************************************************************/

int AVI::skip_chunk(FILE *in)  {
	char chunk_id[5];
	int chunk_size;
	int end_of_chunk;

	read_chars(in,chunk_id,4);
	chunk_size=read_long(in);

	logg->write("Uknown Chunk at %d\n",(int)ftell(in));
	logg->write("-------------------------------\n");
	logg->write("             chunk_id: %s\n",chunk_id);
	logg->write("           chunk_size: %d\n",chunk_size);
	logg->write("\n");

	end_of_chunk=ftell(in)+chunk_size;
	if ((end_of_chunk%4)!=0)
	{
	end_of_chunk=end_of_chunk+(4-(end_of_chunk%4));
	}

	fseek(in,end_of_chunk,SEEK_SET);

	return 0;
	}

/**********************************************************************************************************

**********************************************************************************************************/

int AVI::hex_dump_chunk(FILE *in, int chunk_len) {
	char chars[17];
	int ch,n;

	chars[16]=0;

	for (n=0; n<chunk_len; n++)  {
		if ((n%16)==0)  {
			if (n!=0) logg->write("%s\n", chars);
			logg->write("      ");
			memset(chars, ' ', 16);
		}
		ch=getc(in);
		if (ch==EOF) break;
		logg->write("%02x ", ch); 
		if (ch>=' ' && ch<=126)
		{ chars[n%16]=ch; }
		else
		{ chars[n%16]='.'; }
	}

	if ((n%16)!=0)
	{
		for (ch=n%16; ch<16; ch++) { logg->write("   "); }
	}
	logg->write("%s\n", chars);

	return 0;
}

/**********************************************************************************************************

**********************************************************************************************************/

int AVI::parse_idx1(FILE *in, int chunk_len) {
	struct index_entry_t index_entry;
	int t;
	long count = 0L;

	logg->write("      IDX1\n");
	logg->write("      -------------------------------\n");
	logg->write("      ckid   dwFlags         dwChunkOffset        dwChunkLength\n");

	for (t=0; t<chunk_len/16; t++)  {
		count++;

		if (count<=10)  {
			read_chars(in,index_entry.ckid,4);
			index_entry.dwFlags=read_long(in);
			index_entry.dwChunkOffset=read_long(in);
			index_entry.dwChunkLength=read_long(in);

			logg->write("      %s   0x%08x      0x%08x           0x%08x\n",
					index_entry.ckid,
					index_entry.dwFlags,
					index_entry.dwChunkOffset,
					index_entry.dwChunkLength);
		}

	}

	logg->write("      ...\n");
	logg->write("\n");

	return 0;
}

/**********************************************************************************************************

**********************************************************************************************************/

int AVI::read_avi_header(FILE *in,struct avi_header_t *avi_header) {
	long offset=ftell(in);

	avi_header->TimeBetweenFrames=read_long(in);
	avi_header->MaximumDataRate=read_long(in);
	avi_header->PaddingGranularity=read_long(in);
	avi_header->Flags=read_long(in);
	avi_header->TotalNumberOfFrames=read_long(in);
	avi_header->NumberOfInitialFrames=read_long(in);
	avi_header->NumberOfStreams=read_long(in);
	avi_header->SuggestedBufferSize=read_long(in);
	avi_header->Width=read_long(in);
	avi_header->Height=read_long(in);
	avi_header->TimeScale=read_long(in);
	avi_header->DataRate=read_long(in);
	avi_header->StartTime=read_long(in);
	avi_header->DataLength=read_long(in);

	logg->write("         offset=0x%lx\n",offset);
	logg->write("             TimeBetweenFrames: %d\n",avi_header->TimeBetweenFrames);
	logg->write("               MaximumDataRate: %d\n",avi_header->MaximumDataRate);
	logg->write("            PaddingGranularity: %d\n",avi_header->PaddingGranularity);
	logg->write("                         Flags: %d\n",avi_header->Flags);
	logg->write("           TotalNumberOfFrames: %d\n",avi_header->TotalNumberOfFrames);
	logg->write("         NumberOfInitialFrames: %d\n",avi_header->NumberOfInitialFrames);
	logg->write("               NumberOfStreams: %d\n",avi_header->NumberOfStreams);
	logg->write("           SuggestedBufferSize: %d\n",avi_header->SuggestedBufferSize);
	logg->write("                         Width: %d\n",avi_header->Width);
	logg->write("                        Height: %d\n",avi_header->Height);
	logg->write("                     TimeScale: %d\n",avi_header->TimeScale);
	logg->write("                      DataRate: %d\n",avi_header->DataRate);
	logg->write("                     StartTime: %d\n",avi_header->StartTime);
	logg->write("                    DataLength: %d\n",avi_header->DataLength);

	return 0;
}

/**********************************************************************************************************

**********************************************************************************************************/

void AVI::print_data_handler(unsigned char *handler) {
	int t;

	for (t=0; t<4; t++)
	{
		if ((handler[t]>='a' && handler[t]<='z') ||
				(handler[t]>='A' && handler[t]<='Z') ||
				(handler[t]>='0' && handler[t]<='9'))
		{
			logg->write("%c",handler[t]);
		}
		else
		{
			logg->write("[0x%02x]",handler[t]);
		}
	}
}

/**********************************************************************************************************

**********************************************************************************************************/

int AVI::read_stream_header(FILE *in,struct stream_header_t *stream_header) {
	long offset=ftell(in);

	read_chars(in,stream_header->DataType,4);
	read_chars(in,stream_header->DataHandler,4);
	stream_header->Flags=read_long(in);
	stream_header->Priority=read_long(in);
	stream_header->InitialFrames=read_long(in);
	stream_header->TimeScale=read_long(in);
	stream_header->DataRate=read_long(in);
	stream_header->StartTime=read_long(in);
	stream_header->DataLength=read_long(in);
	stream_header->SuggestedBufferSize=read_long(in);
	stream_header->Quality=read_long(in);
	stream_header->SampleSize=read_long(in);

	logg->write("            offset=0x%lx\n",offset);
	logg->write("                      DataType: %s\n",stream_header->DataType);
	logg->write("                   DataHandler: ");
	print_data_handler((unsigned char *)stream_header->DataHandler);
	logg->write("\n");
	logg->write("                         Flags: %d\n",stream_header->Flags);
	logg->write("                      Priority: %d\n",stream_header->Priority);
	logg->write("                 InitialFrames: %d\n",stream_header->InitialFrames);
	logg->write("                     TimeScale: %d\n",stream_header->TimeScale);
	logg->write("                      DataRate: %d\n",stream_header->DataRate);
	logg->write("                     StartTime: %d\n",stream_header->StartTime);
	logg->write("                    DataLength: %d\n",stream_header->DataLength);
	logg->write("           SuggestedBufferSize: %d\n",stream_header->SuggestedBufferSize);
	logg->write("                       Quality: %d\n",stream_header->Quality);
	logg->write("                    SampleSize: %d\n",stream_header->SampleSize);

	return 0;
}

/**********************************************************************************************************

**********************************************************************************************************/

int AVI::read_stream_format(FILE *in,struct stream_format_t *stream_format) {
	int t,r,g,b;
	long offset=ftell(in);

	stream_format->header_size=read_long(in);
	stream_format->image_width=read_long(in);
	stream_format->image_height=read_long(in);
	stream_format->number_of_planes=read_word(in);
	stream_format->bits_per_pixel=read_word(in);
	stream_format->compression_type=read_long(in);
	stream_format->image_size_in_bytes=read_long(in);
	stream_format->x_pels_per_meter=read_long(in);
	stream_format->y_pels_per_meter=read_long(in);
	stream_format->colors_used=read_long(in);
	stream_format->colors_important=read_long(in);
	stream_format->palette=0;

	if (stream_format->colors_important!=0)  {

		stream_format->palette = (int *)malloc( stream_format->colors_important*sizeof(int) );

		for (t=0; t<stream_format->colors_important; t++)  {
			b=getc(in);
			g=getc(in);
			r=getc(in);
			stream_format->palette[t]=(r<<16)+(g<<8)+b;
		}
	}

	logg->write("            offset=0x%lx\n",offset);
	logg->write("                   header_size: %d\n",stream_format->header_size);
	logg->write("                   image_width: %d\n",stream_format->image_width);
	logg->write("                  image_height: %d\n",stream_format->image_height);
	logg->write("              number_of_planes: %d\n",stream_format->number_of_planes);
	logg->write("                bits_per_pixel: %d\n",stream_format->bits_per_pixel);
	logg->write("              compression_type: %04x (%c%c%c%c)\n",stream_format->compression_type,
			((stream_format->compression_type)&255),
			((stream_format->compression_type>>8)&255),
			((stream_format->compression_type>>16)&255),
			((stream_format->compression_type>>24)&255));
	logg->write("           image_size_in_bytes: %d\n",stream_format->image_size_in_bytes);
	logg->write("              x_pels_per_meter: %d\n",stream_format->x_pels_per_meter);
	logg->write("              y_pels_per_meter: %d\n",stream_format->y_pels_per_meter);
	logg->write("                   colors_used: %d\n",stream_format->colors_used);
	logg->write("              colors_important: %d\n",stream_format->colors_important);

	return 0;
}

/**********************************************************************************************************

**********************************************************************************************************/

int AVI::read_stream_format_auds(FILE *in, struct stream_format_auds_t *stream_format) {
	long offset=ftell(in);

	stream_format->format=read_word(in);
	stream_format->channels=read_word(in);
	stream_format->samples_per_second=read_long(in);
	stream_format->bytes_per_second=read_long(in);
	int block_align=read_word(in);
	stream_format->block_size_of_data=read_word(in);
	stream_format->bits_per_sample=read_word(in);
	//stream_format->extended_size=read_word(in);

	logg->write("            offset=0x%lx\n",offset);
	logg->write("                        format: %d\n",stream_format->format);
	logg->write("                      channels: %d\n",stream_format->channels);
	logg->write("            samples_per_second: %d\n",stream_format->samples_per_second);
	logg->write("              bytes_per_second: %d\n",stream_format->bytes_per_second);
	logg->write("                   block_align: %d\n",block_align);
	logg->write("            block_size_of_data: %d\n",stream_format->block_size_of_data);
	logg->write("               bits_per_sample: %d\n",stream_format->bits_per_sample);


	return 0;
}

/**********************************************************************************************************

**********************************************************************************************************/

int AVI::parse_hdrl_list(FILE *in,struct avi_header_t *avi_header, struct stream_header_t *stream_header, struct stream_format_t *stream_format) {
	struct stream_format_auds_t stream_format_auds;
	struct stream_header_t stream_header_auds;
	char chunk_id[5];
	int chunk_size;
	char chunk_type[5];
	int end_of_chunk;
	int next_chunk;
	long offset=ftell(in);
	int stream_type=0;     // 0=video 1=sound

	read_chars(in, chunk_id, 4);			// "LIST"
	chunk_size = read_long(in);				// 4244
	read_chars(in, chunk_type, 4);			// "strl"

	logg->write("      AVI Header LIST (id=%s size=%d type=%s offset=0x%lx)  {\n",chunk_id,chunk_size,chunk_type,offset);
	//logg->write("      {\n");

	end_of_chunk=ftell(in)+chunk_size-4;
	if ((end_of_chunk%4)!=0) {
		//logg->write("Adjusting end of chunk %d\n", end_of_chunk);
		//end_of_chunk=end_of_chunk+(4-(end_of_chunk%4));
		//logg->write("Adjusting end of chunk %d\n", end_of_chunk);
	}

	if (strcmp(chunk_id,"JUNK")==0) {
		fseek(in,end_of_chunk,SEEK_SET);
		logg->write("      }\n");
		return 0;
	}

	while (ftell(in)<end_of_chunk) {
		long offset=ftell(in);
		read_chars(in,chunk_type,4);
		chunk_size=read_long(in);
		next_chunk=ftell(in)+chunk_size;
		if ((chunk_size%4)!=0)
		{
			//logg->write("Chunk size not a multiple of 4?\n");
			//chunk_size=chunk_size+(4-(chunk_size%4));
		}

		logg->write("         %.4s (size=%d offset=0x%lx)  {\n",chunk_type,chunk_size,offset);
		//logg->write("         {\n");

		if (strcasecmp("strh",chunk_type)==0) {
			long marker=ftell(in);
			char buffer[5];
			read_chars(in,buffer,4); 
			fseek(in,marker,SEEK_SET);

			if (strcmp(buffer, "vids")==0) {
				stream_type=0;
				read_stream_header(in,stream_header);
			}
			else
				if (strcmp(buffer, "auds")==0) {
					stream_type=1;
					read_stream_header(in,&stream_header_auds);
				}
				else {
					logg->write("Unknown stream type %s\n", buffer); 
					return -1;
				}
		}
		else
			if (strcasecmp("strf",chunk_type)==0) {
				if (stream_type==0) {
					read_stream_format(in,stream_format);
				}
				else {
					read_stream_format_auds(in,&stream_format_auds);
				}
			}
			else
				if (strcasecmp("strd",chunk_type)==0) {

				}
				else {
					logg->write("            Unknown chunk type: %s\n",chunk_type);
					// skip_chunk(in);
				}

		logg->write("         }\n");

		fseek(in,next_chunk,SEEK_SET);
	}

	//logg->write("@@@@ %ld %d\n", ftell(in), end_of_chunk);
	logg->write("      }\n");
	fseek(in,end_of_chunk,SEEK_SET);

	return 0;
}

/**********************************************************************************************************

**********************************************************************************************************/

int AVI::parse_hdrl(FILE *in,struct avi_header_t *avi_header, struct stream_header_t *stream_header, struct stream_format_t *stream_format, unsigned int size) {
	char chunk_id[5];
	int chunk_size;
	int end_of_chunk;
	long offset=ftell(in);

	read_chars(in, chunk_id, 4);				// avih
	chunk_size = read_long(in);					// 56

	logg->write("      AVI Header Chunk (id=%s size=%d offset=0x%lx)  {\n",chunk_id,chunk_size,offset);
	//logg->write("      {\n");

	end_of_chunk=ftell(in)+chunk_size;
	if ((end_of_chunk%4)!=0) {
		end_of_chunk=end_of_chunk+(4-(end_of_chunk%4));
	}

	read_avi_header(in,avi_header);
	logg->write("      }\n");

	while(ftell(in)< (long) (offset+size-4)) {
		//logg->write("Should end at 0x%lx  0x%lx\n",offset+size,ftell(in));
		parse_hdrl_list(in,avi_header,stream_header,stream_format);
	}

	return 0;
}

/**********************************************************************************************************

**********************************************************************************************************/

int AVI::parse_riff(FILE *in) {
	char chunk_id[5];
	int chunk_size;
	char chunk_type[5];
	int end_of_chunk,end_of_subchunk;
	struct avi_header_t avi_header;
	struct stream_header_t stream_header;
	struct stream_format_t stream_format={0};
	long offs = 0L;

	logg = new Logger("avi.log");

	offs = ftell(in);					// 0

	read_chars(in, chunk_id, 4);		// "RIFF"
	chunk_size = read_long(in);			// 1.4 gigs
	read_chars(in, chunk_type, 4);		// "AVI"

	logg->write("RIFF Chunk (id=%s size=%d type=%s offset=0x%lx)\n",chunk_id,chunk_size,chunk_type, offs);
	logg->write("{\n");

	if (strcasecmp("RIFF",chunk_id)!=0) {
		logg->write("Not a RIFF file.\n");
		return 1;
	}
	else
		if (strcasecmp("AVI ",chunk_type)!=0) {
			logg->write("Not an AVI file.\n");
			return 1;
		}

	end_of_chunk = ftell(in) + chunk_size - 4;			// 1.4 gigs

	while (ftell(in)<end_of_chunk)  {
		offs = ftell(in);								// 12
		read_chars(in, chunk_id, 4);					// "LIST"
		chunk_size = read_long(in);						// 8830
		end_of_subchunk = ftell(in) + chunk_size;

		if (strcasecmp("JUNK",chunk_id)==0 || strcasecmp("PAD ",chunk_id)==0)  {
			chunk_type[0]=0;
		}
		else  {
			read_chars(in, chunk_type, 4);				// hdr1
		}

		logg->write("\n   New Chunk (id=%s size=%d type=%s offset=0x%lx)  {\n",chunk_id,chunk_size,chunk_type,offs);
		//logg->write(10, 0, 1, "   {\n");

		if (strcasecmp("JUNK",chunk_id)==0 || strcasecmp("PAD ",chunk_id)==0) {
			if ((chunk_size%4)!=0)
			{ chunk_size=chunk_size+(4-(chunk_size%4)); }

			hex_dump_chunk(in, chunk_size);
		}
		else
			if (strcasecmp("INFO",chunk_type)==0) {
				if ((chunk_size%4)!=0)
				{ chunk_size=chunk_size+(4-(chunk_size%4)); }

				hex_dump_chunk(in, chunk_size);
			}
			else
				if (strcasecmp("hdrl",chunk_type)==0) {
					parse_hdrl(in,&avi_header,&stream_header,&stream_format, chunk_size);
					// skip_chunk(in);
				}
				else
					if (strcasecmp("movi",chunk_type)==0) {
						// parse_movi(in);
					}
					else
						if (strcasecmp("idx1",chunk_id)==0) {
							fseek(in,ftell(in)-4,SEEK_SET);
							parse_idx1(in,chunk_size);
						}
						else {
							logg->write("      Unknown chunk at %d (%4s)\n",(int)ftell(in)-8,chunk_type);
							if (chunk_size==0) break;
						}

		fseek(in,end_of_subchunk,SEEK_SET);
		logg->write("   }\n");

	}

	if (stream_format.palette!=0) {
		free(stream_format.palette);
	}

	logg->write(10, 0, 1, "}\n\n");

	return 0;
}								// parse_riff()



