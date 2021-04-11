cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)


project(search_engine_index)


set(SOURCES 	test.cpp
		constraint_solver.cpp
		abstractISR.cpp
		../index/src/Dictionary.cpp		
		../index/src/FileManager.cpp		
		../index/src/ISR.cpp			
		../index/src/Post.cpp		
		../index/src/PostingListBlob.cpp
		../index/src/DocumentInfo.cpp	
		../index/src/IPost.cpp		
		../index/src/IndexConstructor.cpp	
		../index/src/PostingList.cpp
)

set(HEADERS	
		abstractISR.h
		constraint_solver.h
		../index/include/CommonUtil.h		
		../index/include/DictionarySerializer.h	
		../index/include/DocumentsSerializer.h	
		../index/include/FileManager.h		
		../index/include/HashTable.h		
		../index/include/ISR.h			
		../index/include/Post.h			
		../index/include/PostingListBlob.h
		../index/include/Dictionary.h		
		../index/include/DocumentInfo.h		
		../index/include/EndDocSerializer.h	
		../index/include/Global.h		
		../index/include/IPost.h			
		../index/include/IndexConstructor.h	
		../index/include/PostingList.h
		../utility/include/Common.h

)

project(search_engine)

add_executable(search_engine ${SOURCES} ${HEADERS})

target_include_directories(search_engine PRIVATE include ../utility/include)










