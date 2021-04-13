cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

project(search_engine_index)

set(SOURCES 	../constraint_solver/test.cpp
../constraint_solver/abstractISR.cpp
../constraint_solver/constraint_solver.cpp
		src/Dictionary.cpp		
		src/FileManager.cpp		
		src/ISR.cpp			
		src/Post.cpp		
		src/PostingListBlob.cpp
		src/DocumentInfo.cpp	
		src/IPost.cpp		
		src/IndexConstructor.cpp	
		src/PostingList.cpp
)

set(HEADERS	include/CommonUtil.h		
		include/DictionarySerializer.h	
		include/DocumentsSerializer.h	
		include/FileManager.h		
		include/HashTable.h		
		include/ISR.h			
		include/Post.h			
		include/PostingListBlob.h
		include/Dictionary.h		
		include/DocumentInfo.h		
		include/EndDocSerializer.h	
		include/Global.h		
		include/IPost.h			
		include/IndexConstructor.h	
		include/PostingList.h
		../utility/include/Common.h
		../constraint_solver/abstractISR.h
../constraint_solver/constraint_solver.h

		)

project(search_engine)

add_executable(search_engine ${SOURCES} ${HEADERS})

target_include_directories(search_engine PRIVATE include ../utility/include)










