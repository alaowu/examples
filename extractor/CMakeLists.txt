cmake_minimum_required(VERSION 3.0)

project(FFmpegExtractor)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/ffmpeg)
include_directories(${INSTALL_DIR}/include)

add_executable(ffmpegextractor main.c)

target_link_libraries(ffmpegextractor
	${INSTALL_DIR}/lib/libavformat.so
	${INSTALL_DIR}/lib/libavcodec.so
	${INSTALL_DIR}/lib/libavutil.so
	${INSTALL_DIR}/lib/libswresample.so
	${INSTALL_DIR}/extranal/libx264.so
	${INSTALL_DIR}/extranal/libx265.so
	va
	vdpau
	va-drm
	va-x11
	X11
	z
	m
	dl
	pthread
	bz2
	lzma
	)
