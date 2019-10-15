# All variables and this file are optional, if they are not present the PG and the
# makefiles will try to parse the correct values from the file system.
#
# Variables that specify exclusions can use % as a wildcard to specify that anything in
# that position will match. A partial path can also be specified to, for example, exclude
# a whole folder from the parsed paths from the file system
#
# Variables can be specified using = or +=
# = will clear the contents of that variable both specified from the file or the ones parsed
# from the file system
# += will add the values to the previous ones in the file or the ones parsed from the file 
# system
# 
# The PG can be used to detect errors in this file, just create a new project with this addon 
# and the PG will write to the console the kind of error and in which line it is

meta:
	ADDON_NAME = ofxMLTK
	ADDON_DESCRIPTION = ofxMLTK is a Machine Listening Toolkit for OpenFrameworks!
	ADDON_AUTHOR = @mgs
	ADDON_TAGS = "MLTK" "Essentia" "MIR" "DSP" "Streaming" "Dataflow"
	ADDON_URL = http://github.com/mgs/ofxMLTK

common:
	# dependencies with other addons, a list of them separated by spaces 
	# or use += in several lines
	# ADDON_DEPENDENCIES =
	
	# include search paths, this will be usually parsed from the file system
	# but if the addon or addon libraries need special search paths they can be
	# specified here separated by spaces or one per line using +=
	# ADDON_INCLUDES =
	ADDON_INCLUDES_EXCLUDE += libs/gaia/include/3rdparty/Eigen/src/Core/
	
	# any special flag that should be passed to the compiler when using this
	# addon
	# ADDON_CFLAGS =
	
	# any special flag that should be passed to the compiler for c++ files when 
	# using this addon
	# ADDON_CPPFLAGS =
	
	# any special flag that should be passed to the linker when using this
	# addon, also used for system libraries with -lname
	# ADDON_LDFLAGS =
	
	# source files, these will be usually parsed from the file system looking
	# in the src folders in libs and the root of the addon. if your addon needs
	# to include files in different places or a different set of files per platform
	# they can be specified here
	# ADDON_SOURCES =
	
	# source files that will be included as C files explicitly
	# ADDON_C_SOURCES = 
	
	# source files that will be included as header files explicitly
	# ADDON_HEADER_SOURCES = 
	
	# source files that will be included as c++ files explicitly
	# ADDON_CPP_SOURCES = 
	
	# source files that will be included as objective c files explicitly
	# ADDON_OBJC_SOURCES = 
	
	# derines that will be passed to the compiler when including this addon
	# ADDON_DEFINES
	
	# some addons need resources to be copied to the bin/data folder of the project
	# specify here any files that need to be copied, you can use wildcards like * and ?
	# ADDON_DATA = 
	
	# when parsing the file system looking for libraries exclude this for all or
	# a specific platform
	# ADDON_LIBS_EXCLUDE =
	
	# binary libraries, these will be usually parsed from the file system but some 
	# libraries need to passed to the linker in a specific order/
	# 
	# For example in the ofxOpenCV addon we do something like this:
	#
	# ADDON_LIBS =
	# ADDON_LIBS += libs/opencv/lib/linuxarmv6l/libopencv_legacy.a
	# ADDON_LIBS += libs/opencv/lib/linuxarmv6l/libopencv_calib3d.a
	# ...
	
	
linux64:
	# linux only, any library that should be included in the project using
	# pkg-config
	# ADDON_PKG_CONFIG_LIBRARIES =
vs:
	# After compiling copy the following dynamic libraries to the executable directory
	# only windows visual studio
	# ADDON_DLLS_TO_COPY = 
	
linuxarmv6l:
linuxarmv7l:
android/armeabi:	
android/armeabi-v7a:	
osx:
ADDON_LDFLAGS = -Flibs/frameworks -framework VideoToolbox -framework QtCore
ADDON_INCLUDES += libs/frameworks/VideoToolbox.framework/Headers
ADDON_INCLUDES += libs/frameworks/QtCore.framework/Headers
ADDON_LIBS += /usr/lib/libbz2.dylib
ADDON_LIBS += /usr/lib/libiconv.dylib
ADDON_LIBS += libs/fftw3f/lib/osx/libfftw3f.a
ADDON_LIBS += libs/essentia/lib/osx/libessentia.a
ADDON_LIBS += libs/snappy/lib/osx/libsnappy.a
ADDON_LIBS += libs/libvorbis/lib/osx/libvorbis.a
ADDON_LIBS += libs/libvorbis/lib/osx/libvorbisfile.a
ADDON_LIBS += libs/libvorbis/lib/osx/libvorbisenc.a
ADDON_LIBS += libs/libyaml/lib/osx/libyaml.a
ADDON_LIBS += libs/libsamplerate/lib/osx/libsamplerate.a
ADDON_LIBS += libs/taglib/lib/osx/libtag.a
ADDON_LIBS += libs/speex/lib/osx/libspeex.a
ADDON_LIBS += libs/opencore-amr/lib/osx/libopencore-amrwb.a
ADDON_LIBS += libs/opencore-amr/lib/osx/libopencore-amrnb.a
ADDON_LIBS += libs/ffmpeg/lib/osx/libswresample.a
ADDON_LIBS += libs/ffmpeg/lib/osx/libavcodec.a
ADDON_LIBS += libs/ffmpeg/lib/osx/libavdevice.a
ADDON_LIBS += libs/ffmpeg/lib/osx/libavfilter.a
ADDON_LIBS += libs/ffmpeg/lib/osx/libavformat.a
ADDON_LIBS += libs/ffmpeg/lib/osx/libavutil.a
ADDON_LIBS += libs/ffmpeg/lib/osx/libavresample.a
ADDON_LIBS += libs/libgme/lib/osx/libgme.a
ADDON_LIBS += libs/libogg/lib/osx/libogg.a
ADDON_LIBS += libs/opus/lib/osx/libopus.a
ADDON_LIBS += libs/theora/lib/osx/libtheoradec.a
ADDON_LIBS += libs/theora/lib/osx/libtheoraenc.a
ADDON_LIBS += libs/theora/lib/osx/libtheora.a
ADDON_LIBS += libs/openh264/lib/osx/libcommon.a
ADDON_LIBS += libs/openh264/lib/osx/libconsole_common.a
ADDON_LIBS += libs/openh264/lib/osx/libdecoder.a
ADDON_LIBS += libs/openh264/lib/osx/libencoder.a
ADDON_LIBS += libs/openh264/lib/osx/libopenh264.a
ADDON_LIBS += libs/openh264/lib/osx/libprocessing.a
ADDON_LIBS += libs/x264/lib/osx/libx264.a
ADDON_LIBS += libs/x265/lib/osx/libx265.a
ADDON_LIBS += libs/gaia/lib/osx/libgaia2.a
ADDON_LIBS += libs/libvpx/lib/osx/libvpx.a
ADDON_LIBS += libs/sndfile/lib/osx/libsndfile.a
ADDON_LIBS += libs/gnutls/lib/osx/libgnutls.dylib
ADDON_LIBS += libs/chromaprint/lib/osx/libchromaprint.a
ADDON_LIBS += libs/xvid/lib/osx/libxvidcore.a
ADDON_LIBS += libs/lame/lib/osx/libmp3lame.a
ADDON_LIBS += libs/two-lame/lib/osx/libtwolame.a
ios:
