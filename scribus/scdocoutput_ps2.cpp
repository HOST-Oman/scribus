/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include "scdocoutput_ps2.h"

#include "commonstrings.h"
#include "scpage.h"
#include "scpageoutput_ps2.h"
#include "scribuscore.h"
#include "scribusdoc.h"

using namespace std;

ScDocOutput_Ps2::ScDocOutput_Ps2(ScribusDoc* doc, const QString& fileName, vector<int>& pageNumbers, QRect& clip, ScPs2OutputParams& options)
{
	m_doc = doc;
	m_file.setFileName(fileName);
	m_device = &m_file;
	m_pageNumbers = pageNumbers;
	m_clip = clip;
	m_options = options;
	m_status = 0;
	m_author = doc->documentInfo().author();
	m_title  = doc->documentInfo().title();
	m_creator = QString("Scribus ") + QString(VERSION);
}

ScDocOutput_Ps2::~ScDocOutput_Ps2()
{
	if (m_file.isOpen())
		m_file.close();
}

bool ScDocOutput_Ps2::begin()
{
	if (!m_file.open(QIODevice::WriteOnly))
	{
		qDebug()<<"Unable to open file in ScDocOutput_Ps2::begin";
		return false;
	}
	m_stream.setDevice(&m_file);

	m_stream << "%!PS-Adobe-2.0\n";
	m_stream << QString("%%For: %1\n").arg(m_author);
	m_stream << QString("%%Title: %1\n").arg(m_title);
	m_stream << QString("%%Creator: %1\n").arg(m_creator);
	m_stream << QString("%%Pages: %1\n").arg(m_pageNumbers.size());
	m_stream << QString("%%BoundingBox: 0 0 %1 %2\n").arg((int) m_clip.width()).arg((int) m_clip.height());
	m_stream << QString("%%HiResBoundingBox: 0 0 %1 %2\n").arg(m_clip.width()).arg(m_clip.height());
	//TODO Color description
	//m_stream << QString("%%CMYKCustomColor: ");
	//m_stream << QString("%%DocumentCustomColors: ");
	m_stream << "%%BeginSetup\n";
	if (m_options.toGray)
	{
		m_stream << "/setcmykcolor {exch 0.11 mul add exch 0.59 mul add exch 0.3 mul add\n";
		m_stream << "               dup 1 gt {pop 1} if 1 exch sub oldsetgray} bind def\n";
		m_stream << "/setrgbcolor {0.11 mul exch 0.59 mul add exch 0.3 mul add\n";
		m_stream << "              oldsetgray} bind def\n";
	}
	m_stream << QString("<< /PageSize [ %1 %2 ]\n").arg((int) m_clip.width()).arg((int) m_clip.height());
	m_stream << ">> setpagedevice\n";
	m_stream << "%%EndSetup\n";
	return true;
}

void ScDocOutput_Ps2::end()
{
	m_stream << "%%Trailer\n";
	m_stream << "end\n";
	m_stream << "%%EOF\n";
	m_file.close();
}

bool ScDocOutput_Ps2::initializeCmsTransforms()
{
	bool success = false;
	if (!m_options.outputProfile.isEmpty() && QFile::exists(m_options.outputProfile))
	{
		int dcmsflags = 0;
		if (m_doc->BlackPoint)
			dcmsflags |= Ctf_BlackPointCompensation;

		eColorFormat outputDataTypeColors = Format_Undefined;
		eColorFormat outputDataTypeImages = Format_Undefined;
		ScColorMgmtEngine engine(m_doc->colorEngine);
		m_options.hProfile = engine.openProfileFromFile(m_options.outputProfile);
		if (m_options.hProfile.colorSpace() == ColorSpace_Rgb)
		{
			outputDataTypeColors = Format_RGB_16;
			outputDataTypeImages = Format_ARGB_8;
		}
		else if (m_options.hProfile.colorSpace() == ColorSpace_Cmyk)
		{
			outputDataTypeColors = Format_CMYK_16;
			outputDataTypeImages = Format_CMYK_8;
		}
		else
		{
			m_lastError = QObject::tr("Output profile is not supported");
			return false;
		}
		m_options.rgbToOutputColorTransform = engine.createTransform(m_doc->DocInputRGBProf, Format_RGB_16, m_options.hProfile, 
													outputDataTypeColors, m_doc->IntentColors, dcmsflags); 
		m_options.rgbToOutputImageTransform = engine.createTransform(m_doc->DocInputRGBProf, Format_ARGB_8, m_options.hProfile, 
													outputDataTypeImages, m_doc->IntentImages, dcmsflags);
		m_options.cmykToOutputColorTransform = engine.createTransform(m_doc->DocInputRGBProf, Format_CMYK_16, m_options.hProfile, 
													outputDataTypeColors, m_doc->IntentColors, dcmsflags);
		m_options.cmykToOutputImageTransform = engine.createTransform(m_doc->DocInputRGBProf, Format_CMYK_8 , m_options.hProfile, 
													outputDataTypeImages, m_doc->IntentImages, dcmsflags);

		success = (m_options.rgbToOutputColorTransform  && m_options.rgbToOutputImageTransform &&
			       m_options.cmykToOutputColorTransform && m_options.cmykToOutputImageTransform );
		if (!success)
		{
			m_lastError = QObject::tr("An error occurred while initializing icc transforms");
			qWarning( "%s", m_lastError.toLocal8Bit().data() );
			return false;
		}
	}
	return success;
}

ScPageOutput* ScDocOutput_Ps2::createPageOutputComponent(int pageIndex)
{
	ScPageOutput* po = new ScPageOutput_Ps2(m_device, m_doc, pageIndex, m_options);
	return po;
}
