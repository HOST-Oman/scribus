/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include "hunspellpluginimpl.h"
#include "hunspelldialog.h"
#include "langmgr.h"
#include "pageitem.h"
#include "pageitem_textframe.h"
#include "scpaths.h"
#include "scribus.h"
#include "scribusdoc.h"
#include "scribusview.h"
#include "selection.h"
#include "text/specialchars.h"
#include "ui/storyeditor.h"
#include "util.h"


#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>

#ifdef Q_OS_WIN32
#include <windows.h>
#include <shlobj.h>
#endif


// Initialize members here, if any
HunspellPluginImpl::HunspellPluginImpl() : QObject(0)
{
//	numDicts=0;
	m_doc=NULL;
	m_runningForSE=false;
	m_SE=NULL;
}

HunspellPluginImpl::~HunspellPluginImpl()
{
	foreach (HunspellDict* h, hspellerMap)
	{
		delete h;
		h = NULL;
	}
	hspellerMap.clear();
//	numDicts = 0;
}

bool HunspellPluginImpl::run(const QString & target, ScribusDoc* doc)
{
	m_doc=doc;
	bool initOk=initHunspell();
	if (!initOk)
		return false;
	bool spellCheckOk=false;
	if (m_runningForSE)
		spellCheckOk=checkWithHunspellSE();
	else
		spellCheckOk=checkWithHunspell();
	return spellCheckOk;
}

bool HunspellPluginImpl::initHunspell()
{
	bool dictPathFound=LanguageManager::instance()->findSpellingDictionaries(dictionaryPaths);
	if (!dictPathFound)
	{
		qDebug()<<"No preinstalled dictonary paths found";
		return false;
	}
	dictionaryMap.clear();
	LanguageManager::instance()->findSpellingDictionarySets(dictionaryPaths, dictionaryMap);
//	numDicts=dictionaryMap.count();
	if (dictionaryMap.count()==0)
		return false;

	//Initialise one hunspeller for each dictionary found
	QMap<QString, QString>::iterator it = dictionaryMap.begin();
	while (it != dictionaryMap.end())
	{
		//qDebug()<<"hunspell init:"<<it.key()<<it.value();
		hspellerMap.insert(it.key(), new HunspellDict(it.value()+".aff", it.value()+".dic"));
		++it;
	}
	return true;
}

bool HunspellPluginImpl::checkWithHunspell()
{
	PageItem *frameToCheck;

	for( int i = 0; i < m_doc->m_Selection->count(); ++i )
	{
		frameToCheck = m_doc->m_Selection->itemAt(i);
		StoryText *iText=&frameToCheck->itemText;
		parseTextFrame(iText);
		openGUIForTextFrame(iText);
		m_doc->view()->DrawNew();
	}
	return true;
}

bool HunspellPluginImpl::checkWithHunspellSE()
{
	StoryText *iText=&(m_SE->Editor->StyledText);
	parseTextFrame(iText);
	openGUIForStoryEditor(iText);
	m_SE->Editor->updateAll();
	return true;
}

bool HunspellPluginImpl::parseTextFrame(StoryText *iText)
{
	int len = iText->length();
	int currPos = iText->firstWord(), wordStart;
	while (currPos < len)
	{
		wordStart = currPos;
		int wordEnd = iText->endOfWord(wordStart);
		QString word = iText->text(wordStart,wordEnd - wordStart);
		// remove any Ignorable Code Point
		QString tmp = word;
		QString tmp2;
		for (int i =0; i < word.size(); ++i)
		{
			if (!SpecialChars::isIgnorableCodePoint(tmp.at(i).unicode()))
				tmp2 += tmp.at(i);
		}
		word = tmp2;
		QString wordLang = iText->charStyle(wordStart).language();

		if (wordLang.isEmpty())
		{
			const StyleSet<CharStyle> &tmp(m_doc->charStyles());
			for (int i = 0; i < tmp.count(); ++i)
				if(tmp[i].isDefaultStyle())
				{
					//check out why we are getting "German" back here next
					wordLang=tmp[i].language();
					//qDebug()<<"Default char style lang"<<tmp[i].language();
				}
		}
		//we now use the abbreviation
		//wordLang=LanguageManager::instance()->getAbbrevFromLang(wordLang, true, false);
		//A little hack as for some reason our en dictionary from the aspell plugin was not called en_GB or en_US but en, content was en_GB though. Meh.
		if (wordLang=="en")
			wordLang="en_GB";
	//	int spellerIndex=0;
		//qDebug()<<"Word:"<<word<<wordLang;
		if (!dictionaryMap.contains(wordLang))
		{
			//qDebug()<<"Spelling language to match style language NOT installed ("<<wordLang<<")";
			QString altLang=LanguageManager::instance()->getAlternativeAbbrevfromAbbrev(wordLang);
			if (altLang!="")
			{
				//qDebug()<<"altLang"<<altLang<<dictionaryMap.contains(altLang);
				wordLang=altLang;
			}
		}
		else
		{
			//qDebug()<<"Spelling language to match style language IS installed ("<<wordLang<<")";
			int i = 0;
			QMap<QString, QString>::iterator it = dictionaryMap.begin();
			while (it != dictionaryMap.end())
			{
				if (it.key()==wordLang)
					break;
				++i;
				++it;
			}
		//	spellerIndex = i;
		}

		if (hspellerMap.contains(wordLang) && hspellerMap[wordLang]->spell(word)==0)
		{
			//qDebug()<<"hspellerMap.contains(wordLang)"<<hspellerMap.contains(wordLang)<< "hspellerMap[wordLang]->spell(word)"<<hspellerMap[wordLang]->spell(word);
			struct WordsFound wf;
			wf.start=wordStart;
			wf.end=wordEnd;
			wf.w=word;
			wf.changed=false;
			wf.ignore=false;
			wf.changeOffset=0;
			wf.lang = wordLang;
			wf.replacements = hspellerMap[wordLang]->suggest(word);
			wordsToCorrect.append(wf);
		}
		currPos = iText->endOfWord(wordStart);
	}
	return true;
}

bool HunspellPluginImpl::openGUIForTextFrame(StoryText *iText)
{
	HunspellDialog hsDialog(m_doc->scMW(), m_doc, iText);
	hsDialog.set(&dictionaryMap, &hspellerMap, &wordsToCorrect);
	hsDialog.exec();
	if (hsDialog.docChanged())
		m_doc->changed();
	return true;
}

bool HunspellPluginImpl::openGUIForStoryEditor(StoryText *iText)
{
	m_SE->setSpellActive(true);
	HunspellDialog hsDialog(m_SE, m_doc, iText);
	hsDialog.set(&dictionaryMap, &hspellerMap, &wordsToCorrect);
	hsDialog.exec();
	m_SE->setSpellActive(false);
	return true;
}

void HunspellPluginImpl::setRunningForSE(bool rfSE, StoryEditor *sE)
{
	m_runningForSE=rfSE;
	m_SE=sE;
}

