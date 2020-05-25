# Makefile for nmake.
# TODO: Implement header dependencies.

SRCS = \
  cui\Application.cpp \
  cui\Com.cpp \
  cui\CommandLine.cpp \
  cui\CommandLineToken.cpp \
  cui\CommandLineTokenizer.cpp \
  cui\Console.cpp \
  cui\ExceptionLogger.cpp \
  cui\RecordDiffLogger.cpp \
  cui\RecordViewer.cpp \
  elepheye\Command.cpp \
  elepheye\Exception.cpp \
  elepheye\ExceptionProxy.cpp \
  elepheye\RecordName.cpp \
  elepheye\String.cpp \
  elepheye\filter\Csv.cpp \
  elepheye\filter\Rule.cpp \
  elepheye\filter\RuleCommands.cpp \
  elepheye\filter\RuleEntry.cpp \
  elepheye\filter\RuleMatch.cpp \
  elepheye\filter\RuleParser.cpp \
  elepheye\filter\RuleToken.cpp \
  elepheye\filter\RuleTokenizer.cpp \
  elepheye\filter\Validator.cpp \
  elepheye\lib\Field.cpp \
  elepheye\lib\File.cpp \
  elepheye\lib\Handles.cpp \
  elepheye\lib\Hash.cpp \
  elepheye\lib\Privilege.cpp \
  elepheye\lib\RecordComparator.cpp \
  elepheye\lib\ReferenceCounter.cpp \
  elepheye\lib\RegistryKey.cpp \
  elepheye\lib\SkeletonRecord.cpp \
  elepheye\lib\TextInput.cpp \
  elepheye\lib\TextOutput.cpp \
  elepheye\lib\TextPosition.cpp \
  elepheye\lib\TextValue.cpp \
  elepheye\lib\Wildcard.cpp \
  elepheye\source\Csv.cpp \
  elepheye\source\CsvParser.cpp \
  elepheye\source\Filesystem.cpp \
  elepheye\source\FilesystemPath.cpp \
  elepheye\source\FilesystemRecord.cpp \
  elepheye\source\FilesystemScanner.cpp \
  elepheye\source\Registry.cpp \
  elepheye\source\RegistryPath.cpp \
  elepheye\source\RegistryRecord.cpp \
  elepheye\source\RegistryScanner.cpp \
  elepheye\source\Wmi.cpp \
  main.cpp
RCS = cui\resource.rc elepheye\resource.rc
LIBS = advapi32.lib ole32.lib oleaut32.lib user32.lib wbemuuid.lib
OUT = elepheye.exe
FILES = $(OUT)
DESTDIR = ..\bin

!ifdef DEBUG
CPPFLAGS = /D_DEBUG /MTd /Od /RTC1
LDFLAGS = /DEBUG
!else
CPPFLAGS = /DNDEBUG /MT /O2
LDFLAGS = /INCREMENTAL:NO
!endif

CPP = cl
CPPFLAGS = \
  $(CPPFLAGS) \
  /DSTRICT \
  /DUNICODE \
  /DWIN32 \
  /DWINVER=0x0500 \
  /D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES \
  /D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT \
  /D_UNICODE \
  /D_WIN32 \
  /D_WIN32_WINNT=0x0500 \
  /EHsc \
  /I. \
  /W4 \
  /nologo
RC = rc
RCFLAGS = /i.
LD = link
LDFLAGS = $(LDFLAGS) /NOLOGO /OUT:$(OUT)

OBJS = $(SRCS:.cpp=.obj)
RESES = $(RCS:.rc=.res)

all : $(OUT)
$(OUT) : $(OBJS) $(RESES)
	$(LD) $(LDFLAGS) $(OBJS) $(RESES) $(LIBS)
.cpp.obj :
	$(CPP) $(CPPFLAGS) /c /Fo$@ $<
.rc.res :
	$(RC) $(RCFLAGS) /fo$@ $<
clean :
	-del $(OBJS) $(RESES) $(OUT:.exe=.ilk) $(OUT:.exe=.pdb)
distclean : clean
	-del $(OUT)
install : all
	for %%i in ($(FILES)) do copy %%i $(DESTDIR)\^
