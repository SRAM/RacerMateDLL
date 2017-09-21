
@echo **********************************  START OF ../dll/release/post.bat ***************************************

cd release
@copy		racermate.dll		..\..\rm1\RacerMateOne_Source\RacerMateOne
@copy		racermate.dll		..\..\rm1\RacerMateOne_Source\RacerMateOne\bin\Debug
@copy		racermate.dll		..\..\rm1\RacerMateOne_Source\RacerMateOne\bin\Release

@copy		racermate.dll		..\..\rm2\RacerMateOne

@copy		racermate.dll		..\cpp_tester

@echo **********************************  END OF ../dll/release/post.bat ***************************************

