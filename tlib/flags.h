
#ifndef _FLAGS_H_
#define _FLAGS_H_

#pragma pack(push, 1)

//const __int64 StatFlags = 0x1000000000000000L;
//const __int64 StatFlags
//enum StatFlags : ULONG  {
//enum StatFlags : UINT64  {
//enum class StatFlags : __int64  {

namespace StatFlags {                           // packed
	//class StatFlags {
	//enum StatFlags : __int64  {
	//	public:
#ifdef __MACH__
	enum StatFlags : _int64  {
#else
	enum StatFlags : __int64  {
#endif
		Zero			= 0L,

		Time			= (1L << 0),		// Total time of the race.
		LapTime			= (1L << 1),		// Current running total of the lap
		Lap				= (1L << 2),
		Distance		= (1L << 3),		// Total Distance run.
		Lead			= (1L << 4),		// Race order has changed
		Grade			= (1L << 5),
		Wind			= (1L << 6),

		Speed			= (1L << 7),
		Speed_Avg		= (1L << 8),
		Speed_Max		= (1L << 9),

		Watts			= (1L << 10),
		Watts_Avg		= (1L << 11),
		Watts_Max		= (1L << 12),
		Watts_Wkg		= (1L << 13),
		Watts_Load		= (1L << 14),

		HeartRate		= (1L << 15),
		HeartRate_Avg	= (1L << 16),
		HeartRate_Max	= (1L << 17),

		Cadence			= (1L << 18),
		Cadence_Avg		= (1L << 19),
		Cadence_Max		= (1L << 20),

		Calories		= (1L << 21),
		PulsePower		= (1L << 22),
		DragFactor		= (1L << 23),
		SS				= (1L << 24),
		SSLeft			= (1L << 25),
		SSRight			= (1L << 26),
		SSLeftSplit		= (1L << 27),
		SSRightSplit	= (1L << 28),
		SSLeftATA		= (1L << 29),
		SSRightATA		= (1L << 30),
		SSLeft_Avg		= (1L << 31),

		// can't do 64 bit shifts in C++

		SSRight_Avg				= 0x0000000100000000,			// (1L << 32),
		SS_Avg					= SSLeft_Avg | SSRight_Avg,
		SS_Stats				= SS | SSLeft | SSRight | SSLeftSplit | SSRightSplit | SSLeftATA | SSRightATA | SSLeft_Avg | SSRight_Avg,
		HardwareStatusFlags		= 0x0000000200000000,			// (1L << 33),		// Realtime not stored.
		LapDistance				= 0x0000000400000000,			// (1L << 34),		// Used for the course display.
		PercentAT				= 0x0000000800000000,			// (1L << 35);

		FrontGear				= 0x0000001000000000,			// (1L << 36);
		RearGear				= 0x0000002000000000,			// (1L << 37);
		Gearing					= FrontGear | RearGear,
		GearInches				= 0x0000004000000000,			// (1L << 38);
		VelotronOnly			= FrontGear | RearGear | GearInches,
		Drafting				= 0x0000008000000000,			// (1L << 39);

		CadenceTiming			= 0x0000010000000000,			// (1L << 40);
		TSS						= 0x0000020000000000,			// (1L << 41);
		IF						= 0x0000040000000000,			// (1L << 42);
		NP						= 0x0000080000000000,			// (1L << 43);

		TSS_IF_NP				= TSS | IF | NP,
		Bars					= 0x0000100000000000,			// (1L << 44);
		Bars_Shown				= 0x0000200000000000,			// (1L << 45);
		Bars_Avg				= 0x0000400000000000,			// (1L << 46);
		CourseScreenX			= 0x0000800000000000,			// (1L << 47);

		RiderName				= 0x0001000000000000,			// (1L << 48);
		Course					= 0x0002000000000000,			// (1L << 49);
		Order					= 0x0004000000000000,			//  (1L << 50); // Global flag only
		HardwareStatus			= 0x0008000000000000,			// (1L << 51); // Something changed in the hardware connected to that unit.

		Finished				= 0x0010000000000000,			// (1L << 52); // Rider finished the race
		TrackDistance			= 0x0020000000000000,			// (1L << 53);
		Disconnected			= 0x0040000000000000,			// (1L << 54); // True if the unit has been disconnected.
		Calibration				= 0x0080000000000000,			// (1L << 55); // Need to read this from the trainer.

		HeartRateAlarm			= 0x0100000000000000,			// (1L << 56);
		Max						= 0x0200000000000000,			// (1L << 57);
		Mask					= 0x01ffffffffffffff			// Max - 1
	};				// enum StatFlags : __int64 
}					// namespace StatFlags

namespace RawStatFlags  {                           // packed
	//namespace RawStatFlags  {                           // packed
	//class RawStatFlags  {
	//enum RawStatFlags : ULONG  {
	//        private:
	//UINT16 x;
	//	public:
	//		RawStatFlags(UINT16 &_x)  {
	//			x = _x;
	//		}
	//enum : ULONG  {
	enum  {
		//struct RawStatFlags  {
		Zero = 0L,
		Group1 = (1L << 1), // Changes very often
		Group2 = (1L << 2),
		Group3 = (1L << 3),
		Group4 = (1L << 4),
		Group5 = (1L << 5),
		Group6 = (1L << 6),
		Group7 = (1L << 7), // Changes very seldom
		Group8 = (1L << 8),
		Group9 = (1L << 9),
		Group10 = (1L << 10),
		Group11 = (1L << 11), // Seldom changes
		Group12 = (1L << 12), // changes often
		Group_Max = (1L << 13),
		Group_Mask = Group_Max - 1
	};							// enum RawStatFlags : ULONG
}								// namespace RawStatFlags  {                           // packed


#pragma pack(pop)

#endif          // #ifndef _FLAGS_H_

