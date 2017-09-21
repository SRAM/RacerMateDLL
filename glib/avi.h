
#ifndef _AVI_H_
#define _AVI_H_

#include <logger.h>

/*******************************************************************************

http://en.wikipedia.org/wiki/Resource_Interchange_File_Format

Use of the INFO chunk

The optional INFO chunk allows RIFF files to be "tagged" with information falling into a number of predefined categories,
such as copyright ("ICOP"), comments ("ICMT), artist ("IART"), in a standardised way.
These details can be read from a RIFF file even if the rest of the file format is unrecognised.
The standard also allows the use of user-defined fields. Programmers intending to use non-standard fields
should bear in mind that the same non-standard subchunk ID may be used by different applications in different
(and potentially incompatible) ways.

*******************************************************************************/


class AVI  {

	private:
		struct avi_header_t {
		  int TimeBetweenFrames;
		  int MaximumDataRate;
		  int PaddingGranularity;
		  int Flags;
		  int TotalNumberOfFrames;
		  int NumberOfInitialFrames;
		  int NumberOfStreams;
		  int SuggestedBufferSize;
		  int Width;
		  int Height;
		  int TimeScale;
		  int DataRate;
		  int StartTime;
		  int DataLength;
		};

		struct stream_header_t {
		  char DataType[5];
		  char DataHandler[5];
		  int Flags;
		  int Priority;
		  int InitialFrames;
		  int TimeScale;
		  int DataRate;
		  int StartTime;
		  int DataLength;
		  int SuggestedBufferSize;
		  int Quality;
		  int SampleSize;
		};

		struct stream_format_t {
		  int header_size;
		  int image_width;
		  int image_height;
		  int number_of_planes;
		  int bits_per_pixel;
		  int compression_type;
		  int image_size_in_bytes;
		  int x_pels_per_meter;
		  int y_pels_per_meter;
		  int colors_used;
		  int colors_important;
		  int *palette;
		};

		struct stream_header_auds_t {
		  int format_type;
		  int number_of_channels;
		  int sample_rate;
		  int bytes_per_second;
		  int block_size_of_data;
		  int bits_per_sample;
		  int byte_count_extended;
		};

		struct stream_format_auds_t {
		  int header_size;
		  int format;
		  int channels;
		  int samples_per_second;
		  int bytes_per_second;
		  int block_size_of_data;
		  int bits_per_sample;
		  int extended_size;
		};

		struct index_entry_t {
		  char ckid[5];
		  int dwFlags;
		  int dwChunkOffset;
		  int dwChunkLength;
		};

		int hex_dump_chunk(FILE *in, int chunk_len);
		int parse_idx1(FILE *in, int chunk_len);
		int read_avi_header(FILE *in,struct avi_header_t *avi_header);
		void print_data_handler(unsigned char *handler);
		int read_stream_header(FILE *in,struct stream_header_t *stream_header);
		int read_stream_format(FILE *in,struct stream_format_t *stream_format);
		int read_stream_format_auds(FILE *in, struct stream_format_auds_t *stream_format);
		int parse_hdrl_list(FILE *in,struct avi_header_t *avi_header, struct stream_header_t *stream_header, struct stream_format_t *stream_format);
		int parse_hdrl(FILE *in,struct avi_header_t *avi_header, struct stream_header_t *stream_header, struct stream_format_t *stream_format, unsigned int size);
		int parse_riff(FILE *in);
		int skip_chunk(FILE *in);

		char fname[MAX_PATH];
		int init(void);
		int bp;
		FILE *stream;
		char str[256];
		Logger *logg;

	public:
		AVI(const char *_fname);
		~AVI();

};


#endif


