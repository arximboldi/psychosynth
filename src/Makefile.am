
bin_PROGRAMS = psychosynth

psychosynth_SOURCES = \
	common/ArgParser.cpp \
	common/Error.cpp \
	common/AudioBuffer.cpp \
	common/RingAudioBuffer.cpp \
	common/Mutex.cpp \
	common/Misc.cpp \
	table/Table.cpp \
	table/Patcher.cpp \
	table/PatcherDynamic.cpp \
	gui3d/CameraControllerRasko.cpp \
	gui3d/QuitWindow.cpp \
	gui3d/InputManager.cpp \
	gui3d/ElementSelector.cpp \
	gui3d/PsychoSynth3D.cpp \
	gui3d/TaskManager.cpp \
	gui3d/Timer.cpp \
	gui3d/ToggableWindow.cpp \
	gui3d/Element.cpp \
	gui3d/ElementManager.cpp \
	gui3d/WindowList.cpp \
	gui3d/FlatRing.cpp \
	net/NetBuffer.cpp \
	net/NetChannel.cpp \
	net/NetChannelManager.cpp \
	net/NetMessage.cpp \
	net/NetSocket.cpp \
	object/Object.cpp \
	object/ObjectOscillator.cpp \
	object/ObjectMixer.cpp \
	object/ObjectManager.cpp \
	object/ObjectOutput.cpp \
	object/KnownObjects.h \
	output/OutputAlsa.cpp \
	output/OutputOss.cpp \
	psychosynth.cpp

AM_CPPFLAGS = $(all_includes)

psychosynth_CXXFLAGS = $(OGRE_CFLAGS) $(CEGUI_CFLAGS) $(OIS_CFLAGS)
psychosynth_LDFLAGS = $(all_libraries)
psychosynth_LDADD = $(OGRE_LIBS) $(CEGUI_LIBS) $(OIS_LIBS) -lCEGUIOgreRenderer

noinst_HEADERS = \
	common/ArgParser.h \
	common/Error.h \
	common/Packets.h \
	common/Singleton.h \
	common/Thread.h \
	common/Mutex.h \
	common/AudioBuffer.h \
	common/RingAudioBuffer.h \
	common/AudioInfo.h \
        common/MapIterator.h \
	common/Misc.h \
	table/Table.h \
	table/Patcher.h\
	table/PatcherDynamic.h \
	gui3d/CameraController.h \
	gui3d/CameraControllerRasko.h \
	gui3d/CeguiInjecter.h \
	gui3d/QuitWindow.h \
	gui3d/InputManager.h \
	gui3d/PsychoSynth3D.h \
	gui3d/ElementSelector.h\
	gui3d/TaskManager.h\
	gui3d/Timer.h\
	gui3d/ToggableWindow.h\
	gui3d/Element.h\
	gui3d/ElementManager.h\
	gui3d/QueryFlags.h\
	gui3d/WindowList.h\
	gui3d/FlatRing.h\
	net/NetBuffer.h \
	net/NetChannel.h \
	net/NetChannelManager.h \
	net/NetMessage.h \
	net/NetSocket.h \
	object/Object.h \
	object/ObjectOscillator.h \
	object/ObjectManager.h \
	object/ObjectOutput.h \
	object/ObjectMixer.h \
	output/Output.h \
	output/OutputAlsa.h \
	output/OutputOss.h \
	psychosynth.h

