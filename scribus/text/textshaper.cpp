#include "textshaper.h"

#include <hb.h>
#include <hb-ft.h>
#include <hb-icu.h>
#include <unicode/ubidi.h>

#include "scrptrun.h"

#include "glyphcluster.h"
#include "pageitem.h"
#include "scribusdoc.h"
#include "storytext.h"
#include "styles/paragraphstyle.h"


TextShaper::TextShaper(PageItem *item, StoryText &story, int first, bool singlePar)
	: m_item(item)
	, m_story(story)
	, m_firstChar(first)
	, m_singlePar(singlePar)
{ }

TextShaper::TextShaper(StoryText &story, int first)
	: m_item(NULL)
	, m_story(story)
	, m_firstChar(first)
{
	for (int i = m_firstChar; i < m_story.length(); ++i)
	{
		QChar ch = m_story.text(i);
		if (ch == SpecialChars::PARSEP || ch == SpecialChars::LINEBREAK)
			continue;
		QString str(ch);
		m_textMap.insert(i, i);
		m_text.append(str);
	}
}

QList<TextShaper::TextRun> TextShaper::itemizeBiDi(const QString &text)
{
	QList<TextRun> textRuns;
	UBiDi *obj = ubidi_open();
	UErrorCode err = U_ZERO_ERROR;

	UBiDiLevel parLevel = UBIDI_LTR;
	ParagraphStyle style = m_story.paragraphStyle(m_firstChar);
	if (style.direction() == ParagraphStyle::RTL)
		parLevel = UBIDI_RTL;

	ubidi_setPara(obj, text.utf16(), text.length(), parLevel, NULL, &err);
	if (U_SUCCESS(err))
	{
		int32_t count = ubidi_countRuns(obj, &err);
		if (U_SUCCESS(err))
		{
			textRuns.reserve(count);
			for (int32_t i = 0; i < count; i++)
			{
				int32_t start, length;
				UBiDiDirection dir = ubidi_getVisualRun(obj, i, &start, &length);
				textRuns.append(TextRun(start, length, dir));
			}
		}
	}

	ubidi_close(obj);
	return textRuns;
}

QList<TextShaper::TextRun> TextShaper::itemizeScripts(const QString &text, const QList<TextRun> &runs)
{
	QList<TextRun> newRuns;
	ScriptRun scriptrun(text.utf16(), text.length());

	foreach (TextRun run, runs)
	{
		int start = run.start;
		QList<TextRun> subRuns;

		while (scriptrun.next())
		{
			if (scriptrun.getScriptStart() <= start && scriptrun.getScriptEnd() > start)
				break;
		}

		while (start < run.start + run.len)
		{
			int end = qMin(scriptrun.getScriptEnd(), run.start + run.len);
			UScriptCode script = scriptrun.getScriptCode();
			if (run.dir == UBIDI_RTL)
				subRuns.prepend(TextRun(start, end - start, run.dir, script));
			else
				subRuns.append(TextRun(start, end - start, run.dir, script));

			start = end;
			scriptrun.next();
		}

		scriptrun.reset();
		newRuns.append(subRuns);
	}

	return newRuns;
}

QList<TextShaper::FeaturesRun> TextShaper::itemizeFeatures(const TextRun &run)
{
	QList<FeaturesRun> newRuns;
	QList<FeaturesRun> subfeature;
	int start = run.start;

	while (start < run.start + run.len)
	{
		int end = start;
		QStringList startFeatures;
		while (end < run.start + run.len)
		{
			startFeatures = m_story.charStyle(m_textMap.value(start)).fontFeatures().split(",");
			QStringList endFeatures = m_story.charStyle(m_textMap.value(end)).fontFeatures().split(",");
			if (startFeatures != endFeatures)
				break;
			end++;
		}
		subfeature.append(FeaturesRun(start, end - start, startFeatures));
		start = end;
		startFeatures.clear();
	}
	newRuns.append(subfeature);
	return newRuns;
}

QList<TextShaper::TextRun> TextShaper::itemizeStyles(const QMap<int, int> &textMap, const QList<TextRun> &runs)
{
	QList<TextRun> newRuns;

	foreach (TextRun run, runs) {
		int start = run.start;
		QList<TextRun> subRuns;

		while (start < run.start + run.len)
		{
			int end = start;
			while (end < run.start + run.len)
			{
				const CharStyle &startStyle = m_story.charStyle(textMap.value(start));
				const CharStyle &endStyle = m_story.charStyle(textMap.value(end));
				if (!startStyle.equivForShaping(endStyle))
					break;
				end++;
			}
			if (run.dir == UBIDI_RTL)
				subRuns.prepend(TextRun(start, end - start, run.dir, run.script));
			else
				subRuns.append(TextRun(start, end - start, run.dir, run.script));
			start = end;
		}

		newRuns.append(subRuns);
	}

	return newRuns;
}

void TextShaper::buildText(QString &text, QMap<int, int> &textMap)
{
	for (int i = m_firstChar; i < m_story.length(); ++i)
	{
		if (m_singlePar)
		{
			QChar ch = m_story.text(i);
			if (ch == SpecialChars::PARSEP || ch == SpecialChars::LINEBREAK)
				continue;
		}
#if 1 // FIXME HOST: review this insanity
		Mark* mark = m_story.mark(i);
		if ((mark != NULL) && (m_story.hasMark(i)))
		{
			mark->OwnPage = m_item->OwnPage;
			//itemPtr and itemName set to this frame only if mark type is different than MARK2ItemType
			if (!mark->isType(MARK2ItemType))
			{
				mark->setItemPtr(m_item);
				mark->setItemName(m_item->itemName());
			}

			//anchors and indexes has no visible inserts in text
			if (mark->isType(MARKAnchorType) || mark->isType(MARKIndexType))
				continue;

			//set note marker charstyle
			if (mark->isNoteType())
			{
				TextNote* note = mark->getNotePtr();
				if (note == NULL)
					continue;
				mark->setItemPtr(m_item);
				NotesStyle* nStyle = note->notesStyle();
				Q_ASSERT(nStyle != NULL);
				CharStyle currStyle(m_story.charStyle(i));
				QString chsName = nStyle->marksChStyle();
				if (!chsName.isEmpty())
				{
					CharStyle marksStyle(m_item->doc()->charStyle(chsName));
					if (!currStyle.equiv(marksStyle))
					{
						currStyle.setParent(chsName);
						m_story.applyCharStyle(i, 1, currStyle);
					}
				}

				StyleFlag s(m_story.charStyle(i).effects());
				if (mark->isType(MARKNoteMasterType))
				{
					if (nStyle->isSuperscriptInMaster())
						s |= ScStyle_Superscript;
					else
						s &= ~ScStyle_Superscript;
				}
				else
				{
					if (nStyle->isSuperscriptInNote())
						s |= ScStyle_Superscript;
					else
						s &= ~ScStyle_Superscript;
				}
				if (s != m_story.charStyle(i).effects())
				{
					CharStyle haveSuperscript;
					haveSuperscript.setFeatures(s.featureList());
					m_story.applyCharStyle(i, 1, haveSuperscript);
				}
			}
		}

		bool bullet = false;
		if (i == 0 || m_story.text(i - 1) == SpecialChars::PARSEP)
		{
			const ParagraphStyle &style = m_story.paragraphStyle(i);
			if (style.hasBullet() || style.hasNum())
			{
				bullet = true;
				if (mark == NULL || !mark->isType(MARKBullNumType))
				{
					m_story.insertMark(new BulNumMark(), i);
					i--;
					continue;
				}
				if (style.hasBullet())
					mark->setString(style.bulletStr());
				else if (style.hasNum() && mark->getString().isEmpty())
				{
					mark->setString("?");
					m_item->doc()->flag_Renumber = true;
				}
			}
		}

		if (!bullet && mark && mark->isType(MARKBullNumType))
		{
			m_story.removeChars(i, 1);
			i--;
			continue;
		}
#endif
		QString str = m_item->ExpandToken(i);
		if (str.isEmpty())
			str = SpecialChars::ZWNBSPACE;

		const CharStyle &style = m_story.charStyle(i);
		int effects = style.effects() & ScStyle_UserStyles;
		if ((effects & ScStyle_AllCaps) || (effects & ScStyle_SmallCaps))
		{
			QLocale locale(style.language());
			QString upper = locale.toUpper(str);
			if (upper != str)
			{
				if (effects & ScStyle_SmallCaps)
					m_story.setFlag(i, ScLayout_SmallCaps);
				str = upper;
			}
		}

		for (int j = 0; j < str.length(); j++)
			textMap.insert(text.length() + j, i);

		text.append(str);
	}
}

QList<GlyphCluster> TextShaper::shape()
{
	// maps expanded characters to itemText tokens.
	if (m_text.isEmpty())
	{
		buildText(m_text, m_textMap);
	}


	QList<TextRun> bidiRuns = itemizeBiDi(m_text);
	QList<TextRun> scriptRuns = itemizeScripts(m_text, bidiRuns);
	QList<TextRun> textRuns = itemizeStyles(m_textMap, scriptRuns);

	BreakIterator* lineBoundery = StoryText::getLineIterator();
	lineBoundery->setText(m_text.utf16());
	int pos = lineBoundery->first();

	QVector<int> lineBreaks;
	while (pos != BreakIterator::DONE)
	{
		lineBreaks.append(pos);
		pos = lineBoundery->next();
	}

	QList<GlyphCluster> glyphRuns;
	foreach (const TextRun& textRun, textRuns) {
		const CharStyle &style = m_story.charStyle(m_textMap.value(textRun.start));

		const ScFace &scFace = style.font();
		hb_font_t *hbFont = reinterpret_cast<hb_font_t*>(scFace.hbFont());
		if (hbFont == NULL)
			continue;

		hb_font_set_scale(hbFont, style.fontSize(), style.fontSize());
		FT_Face ftFace = hb_ft_font_get_face(hbFont);
		if (ftFace)
			FT_Set_Char_Size(ftFace, style.fontSize(), 0, 72, 0);

		hb_direction_t hbDirection = (textRun.dir == UBIDI_LTR) ? HB_DIRECTION_LTR : HB_DIRECTION_RTL;
		hb_script_t hbScript = hb_icu_script_to_script(textRun.script);
		std::string language = style.language().toStdString();
		hb_language_t hbLanguage = hb_language_from_string(language.c_str(), language.length());

		hb_buffer_t *hbBuffer = hb_buffer_create();
		hb_buffer_add_utf16(hbBuffer, m_text.utf16(), m_text.length(), textRun.start, textRun.len);
		hb_buffer_set_direction(hbBuffer, hbDirection);
		hb_buffer_set_script(hbBuffer, hbScript);
		hb_buffer_set_language(hbBuffer, hbLanguage);
		hb_buffer_set_cluster_level(hbBuffer, HB_BUFFER_CLUSTER_LEVEL_MONOTONE_CHARACTERS);

		QVector<hb_feature_t> hbFeatures;
		QList<FeaturesRun> featuresRuns = itemizeFeatures(textRun);
		foreach (const FeaturesRun& featuresRun, featuresRuns)
		{
			const QStringList& features = featuresRun.features;
			hbFeatures.reserve(features.length());
			foreach (const QString& feature, features) {
				hb_feature_t hbFeature;
				hb_bool_t ok = hb_feature_from_string(feature.toStdString().c_str(), feature.toStdString().length(), &hbFeature);
				if (ok)
				{
					hbFeature.start = featuresRun.start;
					hbFeature.end = featuresRun.len + featuresRun.start;
					hbFeatures.append(hbFeature);
				}
			}
		}

		hb_shape(hbFont, hbBuffer, hbFeatures.data(), hbFeatures.length());

		unsigned int count = hb_buffer_get_length(hbBuffer);
		hb_glyph_info_t *glyphs = hb_buffer_get_glyph_infos(hbBuffer, NULL);
		hb_glyph_position_t *positions = hb_buffer_get_glyph_positions(hbBuffer, NULL);

		glyphRuns.reserve(glyphRuns.size() + count);
		for (size_t i = 0; i < count; )
		{
			uint32_t firstCluster = glyphs[i].cluster;
			uint32_t nextCluster = firstCluster;
			if (hbDirection == HB_DIRECTION_LTR)
			{
				size_t j = i + 1;
				while (j < count && nextCluster == firstCluster)
				{
					nextCluster = glyphs[j].cluster;
					j++;
				}
				if (j == count && nextCluster == firstCluster)
					nextCluster = textRun.start + textRun.len;
			}
			else
			{
				int j = i - 1;
				while (j >= 0 && nextCluster == firstCluster)
				{
					nextCluster = glyphs[j].cluster;
					j--;
				}
				if (j <= 0 && nextCluster == firstCluster)
					nextCluster = textRun.start + textRun.len;
			}

			assert(m_textMap.contains(firstCluster));
			assert(m_textMap.contains(nextCluster - 1));
			int firstChar = m_textMap.value(firstCluster);
			int lastChar = m_textMap.value(nextCluster - 1);

			QChar ch = m_story.text(firstChar);
			LayoutFlags flags = m_story.flags(firstChar);
			const CharStyle& charStyle(m_story.charStyle(firstChar));
			const StyleFlag& effects = charStyle.effects();

			GlyphCluster run(&charStyle, flags, firstChar, lastChar, m_story.object(firstChar), glyphRuns.length());

			if (textRun.dir == UBIDI_RTL)
				run.setFlag(ScLayout_RightToLeft);

			if (lineBreaks.contains(firstCluster))
				run.setFlag(ScLayout_LineBoundry);

			if (SpecialChars::isExpandingSpace(ch))
				run.setFlag(ScLayout_ExpandingSpace);

			if (effects & ScStyle_Underline)
				run.setFlag(ScLayout_Underlined);
			if (effects & ScStyle_UnderlineWords && !ch.isSpace())
				run.setFlag(ScLayout_Underlined);

			run.setScaleH(charStyle.scaleH() / 1000.0);
			run.setScaleV(charStyle.scaleV() / 1000.0);

			GlyphLayout* lastGlyph = NULL;
			while (i < count && glyphs[i].cluster == firstCluster)
			{
				GlyphLayout gl;
				gl.glyph = glyphs[i].codepoint;
				if (gl.glyph == 0 ||
				    (ch == SpecialChars::LINEBREAK || ch == SpecialChars::PARSEP ||
				     ch == SpecialChars::FRAMEBREAK || ch == SpecialChars::COLBREAK))
				{
					gl.glyph = scFace.emulateGlyph(ch.unicode());
				}

				if (gl.glyph < ScFace::CONTROL_GLYPHS)
				{
					gl.xoffset = positions[i].x_offset / 10.0;
					gl.yoffset = -positions[i].y_offset / 10.0;
					gl.xadvance = positions[i].x_advance / 10.0;
					gl.yadvance = positions[i].y_advance / 10.0;
				}

				if (m_story.hasMark(firstChar))
				{
					GlyphLayout control;
					control.glyph = SpecialChars::OBJECT.unicode() + ScFace::CONTROL_GLYPHS;
					run.append(control);
				}

				if (SpecialChars::isExpandingSpace(ch))
					gl.xadvance *= run.style().wordTracking();

				if (m_story.hasObject(firstChar))
					gl.xadvance = m_story.object(firstChar)->width() + m_story.object(firstChar)->lineWidth();

				if ((effects & ScStyle_Superscript) || (effects & ScStyle_Subscript))
				{
					double scale;
					double asce = style.font().ascent(style.fontSize() / 10.0);
					if (effects & ScStyle_Superscript)
					{
						gl.yoffset -= asce * m_item->doc()->typographicPrefs().valueSuperScript / 100.0;
						scale = qMax(m_item->doc()->typographicPrefs().scalingSuperScript / 100.0, 10.0 / style.fontSize());
					}
					else // effects & ScStyle_Subscript
					{
						gl.yoffset += asce * m_item->doc()->typographicPrefs().valueSubScript / 100.0;
						scale = qMax(m_item->doc()->typographicPrefs().scalingSubScript / 100.0, 10.0 / style.fontSize());
					}

					run.setScaleH(run.scaleH() * scale);
					run.setScaleV(run.scaleV() * scale);
				}

				if (run.hasFlag(ScLayout_SmallCaps))
				{
					double smallcapsScale = m_item->doc()->typographicPrefs().valueSmallCaps / 100.0;
					run.setScaleH(run.scaleH() * smallcapsScale);
					run.setScaleV(run.scaleV() * smallcapsScale);
				}

				if (run.scaleH() == 0.0)
				{
					gl.xadvance = 0.0;
					run.setScaleH(1.0);
				}

				run.append(gl);
				lastGlyph = &gl;
				i++;
			}

			// Apply CJK spacing according to JIS X4051
			if (lastGlyph && lastChar + 1 < m_story.length())
			{
				double halfEM = run.style().fontSize() / 10 / 2;
				double quarterEM = run.style().fontSize() / 10 / 4;

				int currStat = SpecialChars::getCJKAttr(m_story.text(lastChar));
				int nextStat = SpecialChars::getCJKAttr(m_story.text(lastChar + 1));
				if (currStat != 0)
				{	// current char is CJK
					if (nextStat == 0 && !SpecialChars::isBreakingSpace(m_story.text(lastChar + 1))) {
						switch(currStat & SpecialChars::CJK_CHAR_MASK) {
						case SpecialChars::CJK_KANJI:
						case SpecialChars::CJK_KANA:
						case SpecialChars::CJK_NOTOP:
							lastGlyph->xadvance += quarterEM;
						}
					} else {	// next char is CJK, too
						switch(currStat & SpecialChars::CJK_CHAR_MASK) {
						case SpecialChars::CJK_FENCE_END:
							switch(nextStat & SpecialChars::CJK_CHAR_MASK) {
							case SpecialChars::CJK_FENCE_BEGIN:
							case SpecialChars::CJK_FENCE_END:
							case SpecialChars::CJK_COMMA:
							case SpecialChars::CJK_PERIOD:
							case SpecialChars::CJK_MIDPOINT:
								lastGlyph->xadvance -= halfEM;
							}
							break;
						case SpecialChars::CJK_COMMA:
						case SpecialChars::CJK_PERIOD:
							switch(nextStat & SpecialChars::CJK_CHAR_MASK) {
							case SpecialChars::CJK_FENCE_BEGIN:
							case SpecialChars::CJK_FENCE_END:
								lastGlyph->xadvance -= halfEM;;
							}
							break;
						case SpecialChars::CJK_MIDPOINT:
							switch(nextStat & SpecialChars::CJK_CHAR_MASK) {
							case SpecialChars::CJK_FENCE_BEGIN:
								lastGlyph->xadvance -= halfEM;
							}
							break;
						case SpecialChars::CJK_FENCE_BEGIN:
							int prevStat = SpecialChars::getCJKAttr(m_story.text(lastChar - 1));
							if ((prevStat & SpecialChars::CJK_CHAR_MASK) == SpecialChars::CJK_FENCE_BEGIN)
							{
								lastGlyph->xadvance -= halfEM;
								lastGlyph->xoffset -= halfEM;
							}
							else
							{
								run.setFlag(ScLayout_CJKFence);
							}
							break;
						}
					}
				} else {	// current char is not CJK
					if (nextStat != 0 && !SpecialChars::isBreakingSpace(m_story.text(lastChar))) {
						switch(nextStat & SpecialChars::CJK_CHAR_MASK) {
						case SpecialChars::CJK_KANJI:
						case SpecialChars::CJK_KANA:
						case SpecialChars::CJK_NOTOP:
							// use the size of the current char instead of the next one
							lastGlyph->xadvance += quarterEM;
						}
					}
				}
			}

			glyphRuns.append(run);
		}
		hb_buffer_destroy(hbBuffer);

	}

	return glyphRuns;
}
