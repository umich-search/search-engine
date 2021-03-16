test:  HtmlParserDemo NYTimes.html
#	./HtmlParserDemo t NYTimes.html > TitleWords.txt
#	diff TitleWords.correct.txt TitleWords.txt
#	./HtmlParserDemo w NYTimes.html > Words.txt
#	diff -Z Words.correct.txt Words.txt
#	./HtmlParserDemo a NYTimes.html > Links.txt
#	diff -Z Links.correct.txt Links.txt
#	./HtmlParserDemo b NYTimes.html > Base.txt
#	diff -Z Base.correct.txt Base.txt
	./HtmlParserDemo NYTimes.html > my.nytime.out.txt
	diff nytime.out.txt my.nytime.out.txt
	./HtmlParserDemo lotsOfjunk.html > my.lots.out.txt
	diff lots.out.txt my.lots.out.txt
	./HtmlParserDemo broken.htm > my.broken.out.txt
	diff broken.out.txt my.broken.out.txt

HtmlParserDemo:  HtmlTags.h HtmlTags.cpp HtmlParser.h HtmlParser.cpp HtmlParserDemo.cpp
	g++ -std=c++11 HtmlParserDemo.cpp HtmlTags.cpp HtmlParser.cpp -DLinux -o HtmlParserDemo

clean:
	rm -f HtmlParserDemo TitleWords.txt Words.txt Links.txt Base.txt
