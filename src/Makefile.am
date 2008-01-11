
bin_PROGRAMS = psychosynth

psychosynth_SOURCES = \
	common/ArgParser.cpp \
	common/Error.cpp \
	common/AudioBuffer.cpp \
	common/RingAudioBuffer.cpp \
	common/Mutex.cpp \
	common/Misc.cpp \
	common/Logger.cpp \
	table/Table.cpp \
	table/Patcher.cpp \
	table/PatcherDynamic.cpp \
	gui3d/CameraControllerRasko.cpp \
	gui3d/QuitWindow.cpp \
	gui3d/InputManager.cpp \
	gui3d/SelectorWindow.cpp \
	gui3d/PsychoSynth3D.cpp \
	gui3d/TaskManager.cpp \
	gui3d/Timer.cpp \
	gui3d/ToggableWindow.cpp \
	gui3d/Element.cpp \
	gui3d/ElemMainComponent.cpp \
	gui3d/ElemSecondComponent.cpp \
	gui3d/ElementManager.cpp \
	gui3d/ElementProperties.cpp \
	gui3d/TheElements.cpp \
	gui3d/WindowList.cpp \
	gui3d/FlatRing.cpp \
	gui3d/Connection.cpp \
	gui3d/OgreMisc.cpp \
	gui3d/NetworkWindow.cpp \
	gui3d/RecordWindow.cpp \
	gui3d/GuiLogSink.cpp \
	object/Object.cpp \
	object/ObjectOscillator.cpp \
	object/ObjectMixer.cpp \
	object/ObjectAudioMixer.cpp \
	object/ObjectControlMixer.cpp \
	object/ObjectManager.cpp \
	object/ObjectOutput.cpp \
	object/ObjectLFO.cpp \
	object/ObjectAudioOscillator.cpp \
	object/ObjectFilter.cpp \
	object/Filter.cpp \
	object/WaveTable.cpp \
	object/Oscillator.cpp \
	net/OSCBroadcast.cpp \
	net/OSCController.cpp \
	net/OSCServer.cpp \
	net/OSCClient.cpp \
	output/OutputJack.cpp \
	output/OutputAlsa.cpp \
	output/OutputOss.cpp \
	output/OutputWave.cpp \
	psychosynth.cpp

AM_CPPFLAGS = $(all_includes)

psychosynth_CXXFLAGS = \
	$(OGRE_CFLAGS) \
	$(CEGUI_CFLAGS) \
	$(OIS_CFLAGS) \
	$(CEGUIOGRE_CFLAGS) \
	$(ALSA_CFLAGS) \
	$(JACK_CFLAGS) \
	$(LIBLO_CFLAGS) \
	$(AUDIOFILE_CFLAGS)

psychosynth_LDFLAGS = $(all_libraries)
psychosynth_LDADD = \
	$(OGRE_LIBS) \
	$(CEGUI_LIBS) \
	$(OIS_LIBS) \
	$(CEGUIOGRE_LIBS)\
	$(ALSA_LIBS) \
	$(JACK_LIBS) \
	$(LIBLO_LIBS) \
	$(AUDIOFILE_LIBS)

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
	common/Logger.h \
	table/Table.h \
	table/Patcher.h\
	table/PatcherDynamic.h \
	gui3d/CameraController.h \
	gui3d/CameraControllerRasko.h \
	gui3d/CeguiInjecter.h \
	gui3d/QuitWindow.h \
	gui3d/InputManager.h \
	gui3d/PsychoSynth3D.h \
	gui3d/SelectorWindow.h\
	gui3d/TaskManager.h\
	gui3d/Timer.h\
	gui3d/ToggableWindow.h\
	gui3d/Element.h\
	gui3d/ElemMainComponent.h \
	gui3d/ElemSecondComponent.h \
	gui3d/ElementManager.h\
	gui3d/ElementProperties.h \
	gui3d/TheElements.h \
	gui3d/QueryFlags.h\
	gui3d/WindowList.h\
	gui3d/FlatRing.h\
	gui3d/Connection.h\
	gui3d/OgreMisc.h \
	gui3d/NetworkWindow.h \
	gui3d/RecordWindow.h \
	gui3d/GuiLogSink.h \
	object/Object.h \
	object/ObjectOscillator.h \
	object/ObjectManager.h \
	object/ObjectOutput.h \
	object/ObjectMixer.h \
	object/ObjectAudioMixer.h \
	object/ObjectControlMixer.h \
	object/ObjectLFO.h \
	object/ObjectAudioOscillator.h \
	object/ObjectFilter.h \
	object/Filter.h \
	object/WaveTable.h \
	object/Oscillator.cpp \
	object/KnownObjects.h \
	net/OSCBroadcast.h \
	net/OSCServer.h \
	net/OSCClient.h \
	net/OSCServerLogger.h \
	net/OSCClientLogger.h \
	net/OSCController.h \
	net/OSCMisc.h \
	net/OSCProtocol.h \
	output/Output.h \
	output/OutputJack.h \
	output/OutputAlsa.h \
	output/OutputOss.h \
	output/OutputWave.h \
	psychosynth.h

