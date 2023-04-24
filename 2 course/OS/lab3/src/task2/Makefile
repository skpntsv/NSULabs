all: manager hardlinks

manager:
	gcc -o dirmanager ./source/main.c -Wall

hardlinks: dirmanager
	ln dirmanager createDir
	ln dirmanager printDir
	ln dirmanager removeDir
	ln dirmanager createFile
	ln dirmanager printFile
	ln dirmanager removeFile
	ln dirmanager createSymbLink
	ln dirmanager printSymbLink
	ln dirmanager printLinkFile
	ln dirmanager removeSymLink
	ln dirmanager createHardLink
	ln dirmanager removeHardLink
	ln dirmanager showInfo
	ln dirmanager changeMod
	rm dirmanager

clean:
	rm -f dirmanager
	rm -f createDir printDir removeDir 
	rm -f createFile printFile removeFile
	rm -f createSymbLink printSymbLink printLinkFile removeSymLink
	rm -f createHardLink removeHardLink
	rm -f showInfo changeMod
