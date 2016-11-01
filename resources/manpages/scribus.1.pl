.TH "scribus" "1" "Updated 2004-12-16" "" ""
.SH "NAZWA"
scribus \- program do sk�adu publikacji w trybie WYSIWYG dla �rodowiska X11 (wolne oprogramowanie na licencji GNU GPL)
.SH "SK�ADNIA"
scribus [\-h|\-\-help] [\-v|\-\-version] [\-l|\-\-lang j�zyk] [\-la|\-\-langs\-available] [\-f|\-\-file|\-\-] [nazwa pliku]
.SH "OPIS"
Scribus jest programem do sk�adu publikacji b�d�cym wolnym oprogramowaniem. Umo�liwia edycj� w trybie WYSIWYG, doskona�y eksport do formatu PDF i szeroki wyb�r opcji importu i eksportu.

Wyczerpuj�c� dokumentacj� znale�� mo�na na stronie dokumentacyjnej Scribusa
.I http://docs.scribus.net/
lub w postaci plik�w pomocy dost�pnych w programie za po�rednictwem opcji w menu \'Pomoc\'.

Niniejszy podr�cznik systemowy zawiera jedynie skr�towy opis niekt�rych aspekt�w zastosowania programu. Jego g��wnym zadaniem jest umo�liwienie u�ytkownikowi szybkiego i �atwego znalezienia pe�nej dokumentacji.
.SH "OPCJE"
Wszystkie opcje programu zawarte s� w instrukcji obs�ugi, kt�r� wywo�a� mo�na poleceniem:
.B scribus \-\-help

.TP 
.B \-l, \-\-lang xx
Nadpisuje locale systemowe i startuje Scribusa w j�zyku xx. Kod j�zyka odpowiada kodom w standardzie POSIX, u�ywanym w zmiennych �rodowiskowych LANG i LC_ALL. Na przyk�ad j�zyk polski mo�na wybra� podaj�c 'pl' lub 'pl_PL', j�zyk angielski u�ywaj�c 'en' (generyczny angielski), 'en_GB' (brytyjski angielski) lub 'en_US' (ameryka�ski angielski). W podobny spos�b mo�na wybra� inne j�zyki, np. niemiecki u�ywaj�c kod�w 'de' lub 'de_DE'.
.TP 
.B \-la, \-\-langs\-available
Wy�wietla list� j�zyk�w, w kt�rych dost�pne jest t�umaczenie interfejsu. Aby wybra� dany j�zyk, wystartuj Scribusa poleceniem 'scribus \-l xx', w kt�rym xx oznacza kod j�zyka, lub zmie� odpowiednie zmienne �rodowiskowe w spos�b opisany poni�ej.
.TP 
.B \-v, \-\-version
Wy�wietla numer wersji Scribusa.
.TP 
.B \-f, \-\-file
Otwiera podany plik. Mo�na r�wnie� zamiast tego poda� nazw� pliku jako niekwalifikowany argument, ale je�li nazwa zaczyna si� od \-, nale�y poprzedzi� j� \-\-, np. 'scribus \-\- \-myfile.sla'.
.TP 
.B \-h, \-\-help
Wy�wietla kr�tk� instrukcj� obs�ugi.
.SH "�RODOWISKO"
Scribus uwzgl�dnia stanardowe zmienne �rodowiskowe zdefiniowane w locale. Inne zmienne �rodowiskowe mog� by� u�ywane przez programy i bibioteki wymagane przez Scribusa, np. QT.
.I http://docs.scribus.net/
oraz dokumentacja zawarta w programie mo�e wymienia� inne zmienne, u�ywane przez Scribusa i wymagane przez niego programy i biblioteki.
.TP 
.B LC_ALL, LC_MESSAGES, LANG
POSIX locale. Zobacz
.I locale(1).
Scribus stosuje te zmienne w kolejno�ci podanej powy�ej, aby wybra� j�zyk (w��cznie z t�umaczeniem interfejsu), kt�ry ma by� u�yty. Je�li Scribus nie znajdzie �adnej z tych zmiennych, u�yje ustawie� locale stosowanych przez bibliotek� QT.
.TP 
.B PATH
Scribus mo�e poszuka� w PATH zewn�trznych narz�dzi, je�li �cie�ki do nich nie zosta�y zdefiniowane w programie. Aktualnie ma to miejsce, kiedy zainstalowanych jest par� kopii gs(1). Mo�na poda� w�a�ciw� �cie�k� do gs(1) w ustawieniach Scribusa i obej�� w ten spos�b przeszukiwanie 
.B PATH
.
.PP 
Poni�ej znajduje si� zestawienie kilku najwa�niejszych zmiennych �rodowiskowych u�ywanych przez inne programy, jednak w przypadku problem�w nale�y zajrze� do oryginalnej dokumentacji programu.
.TP 
.B GS_FONTPATH
�cie�ka do czcionek u�ywana przez GhostScript. Ma wp�yw na szukanie czcionek dla GhostScriptu, kt�rego Scribus u�ywa do wielu operacji na plikach PostScript. Dodawaj nowe katalogi zawieraj�ce czcionki rozdzielaj�c je �rednikiem, aby umo�liwi� GhostScriptowi znalezienie czcionek po�o�onych w niestandardowych katalogach. Zobacz gs(1) i dokumentacj� GhostScriptu w formacie HTML, aby znale�� wi�cej informacji.
.TP 
.B GS_LIB
�cie�ka do biblioteki GhostScriptu. GhostScript szuka w tej �cie�ce plik�w Fontmap. Podobnie jak
.B GS_FONTPATH
jest to lista katalog�w rozdzielonych �rednikami. Zazwyczaj u�ywa si� zamiast tego zmiennej 
.B GS_FONTPATH
, jednak utworzenie plik�w Fontmap i u�ycie zmiennej 
.B GS_LIB
mo�e przy�pieszy� dzia�anie, je�li u�ywamy 
.I bardzo du�o
czcionek. Zobacz gs(1) i dokumentacj� GhostScriptu w formacie HTML, aby uzyska� wi�cej informacji.
.SH "PLIKI"
.TP 
.B $HOME/.scribus/
Ustawienia u�ytkownika zapisywane s� w katalogu $HOME/.scribus/. Wi�kszo�� innych �cie�ek mo�na zmieni� w ustawieniach Scribusa.
.TP 
.B $HOME/.scribus/scribus.rc
Ustawienia Scribusa we w�asnym formacie xml. Mo�e zosta� zast�piony nowym formatem pliku ustawie�.
.TP 
.B $HOME/.scribus/prefs.xml
Plik ustawie� w nowym formacie. W momencie tworzenia niniejszego dokumentu (wersja 1.2.x) plik ten jest jeszcze prawie ca�kowicie niewykorzystywany.
Pliki konfiguracyjne powi�zanych program�w:
.TP 
.B $HOME/.fonts
.TP 
.B /etc/fonts
Fontconfig, biblioteka u�ywana przez Scribusa do znajdowania czcionek, u�ywa plik�w konfiguracyjnych znajduj�cych si� normalnie w katalogu /etc/fonts, g��wnie plik�w /etc/fonts/fonts.conf i /etc/fonts/local.conf. Mo�e r�wnie� u�ywa� pliku konfiguracyjnego w $HOME/.fonts/ . Zobacz fonts.conf(5) i dokumentacj� biblioteki fontconfig, aby uzyska� wi�cej informacji. Zwr�� uwag�, �e Scribus mo�e r�wnie� u�ywa� swoich w�asnych �cie�ek do czcionek - zobacz  menu Ustawienia i pomoc wewn�trz Scribusa.
.SH "ZOBACZ TAK�E"
Strona z dokumentacj� Scribusa http://docs.scribus.net/ i strona domowa http://www.scribus.net/

gs(1), dokumentacja gs w html i strona http://www.ghostscript.com/

.PP 
Biblioteka QT \- http://www.trolltech.com/

fonts\-conf(5) - informacje o konfiguracji biblioteki FontConfig
.SH "POKREWNE OPROGRAMOWANIE I PARTNERZY"
inkscape(1) \- http://inkscape.org/

OpenClipArt \- http://openclipart.org/

Fontmatrix \- http://fontmatrix.be/

Inni partnerzy mog� by� wymienieni w stopce strony http://www.scribus.net/

gimp(1) \- http://www.gimp.org/

.SH "B��DY"
Strona
.I 
http://bugs.scribus.net/
zawiera system �ledzenia b��d�w Scribusa, u�ywany do zg�aszania b��d�w i propozycji zmian.
.B Je�li masz zamiar zg�osi� b��d, przeszukaj najpierw baz� danych.
.SH "AUTORZY"
W menu \'Pomoc\' w opcji \'O Scribusie\' znale�� mo�na list� autor�w, t�umaczy i os�b zaanga�owanych w rozw�j programu.
