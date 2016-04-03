#include "textshaper.h"

#include <hb.h>
#include <hb-ft.h>
#include <hb-icu.h>
#include <unicode/ubidi.h>
#include <QTextBoundaryFinder>

#include "scrptrun.h"

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

QList<TextShaper::TextRun> TextShaper::itemizeBiDi(QString &text)
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

QList<TextShaper::TextRun> TextShaper::itemizeScripts(QString &text, QList<TextRun> &runs)
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

QList<TextShaper::TextRun> TextShaper::itemizeStyles(QMap<int, int> &textMap, QList<TextRun> &runs)
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
		if (m_story.hasMark(i))
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
		if (effects & ScStyle_AllCaps)
			str = str.toUpper();

		for (int j = 0; j < str.length(); j++)
			textMap.insert(text.length() + j, i);

		text.append(str);
	}
}

QList<GlyphRun> TextShaper::shape()
{
	// maps expanded characters to itemText tokens.

	buildText(m_text, m_textMap);

	QTextBoundaryFinder lineBoundery(QTextBoundaryFinder::Line, m_text);

	QList<TextRun> bidiRuns = itemizeBiDi(m_text);
	QList<TextRun> scriptRuns = itemizeScripts(m_text, bidiRuns);
	QList<TextRun> textRuns = itemizeStyles(m_textMap, scriptRuns);

	QList<GlyphRun> glyphRuns;
	foreach (TextRun textRun, textRuns) {
		const CharStyle &style = m_story.charStyle(m_textMap.value(textRun.start));
		int effects = style.effects() & ScStyle_UserStyles;

		FT_Set_Char_Size(style.font().ftFace(), style.fontSize(), 0, 72, 0);

		// TODO: move to ScFace
		hb_font_t *hbFont = hb_ft_font_create_referenced(style.font().ftFace());
		hb_buffer_t *hbBuffer = hb_buffer_create();

		hb_direction_t hbDirection = (textRun.dir == UBIDI_LTR) ? HB_DIRECTION_LTR : HB_DIRECTION_RTL;
		hb_script_t hbScript = hb_icu_script_to_script(textRun.script);
		std::string language = style.language().toStdString();
		hb_language_t hbLanguage = hb_language_from_string(language.c_str(), language.length());

		hb_buffer_clear_contents(hbBuffer);
		hb_buffer_add_utf16(hbBuffer, m_text.utf16(), m_text.length(), textRun.start, textRun.len);
		hb_buffer_set_direction(hbBuffer, hbDirection);
		hb_buffer_set_script(hbBuffer, hbScript);
		hb_buffer_set_language(hbBuffer, hbLanguage);
		hb_buffer_set_cluster_level(hbBuffer, HB_BUFFER_CLUSTER_LEVEL_MONOTONE_CHARACTERS);

		QStringList features = style.fontFeatures().split(",");
		QVector<hb_feature_t> hbFeatures;
		hbFeatures.reserve(features.length());
		foreach (QString feature, features) {
			hb_feature_t hbFeature;
			hb_bool_t ok = hb_feature_from_string(feature.toStdString().c_str(), feature.toStdString().length(), &hbFeature);
			if (ok)
				hbFeatures.append(hbFeature);
		}

		hb_shape_full(hbFont, hbBuffer, hbFeatures.data(), hbFeatures.length(), NULL);

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

			GlyphRun run(&charStyle, flags, firstChar, lastChar, m_story.object(firstChar), glyphRuns.length());
			if (textRun.dir == UBIDI_RTL)
				run.setFlag(ScLayout_RightToLeft);
			lineBoundery.setPosition(firstCluster);
			if (lineBoundery.isAtBoundary())
				run.setFlag(ScLayout_LineBoundry);
			if (SpecialChars::isExpandingSpace(ch))
				run.setFlag(ScLayout_ExpandingSpace);

			while (i < count && glyphs[i].cluster == firstCluster)
			{
				GlyphLayout gl;
				gl.glyph = glyphs[i].codepoint;
				// indirect way to call ScFace::emulateGlyph() as it is private.
				if (gl.glyph == 0)
					gl.glyph = style.font().char2CMap(ch);
				gl.xoffset = positions[i].x_offset / 10.0;
				gl.yoffset = -positions[i].y_offset / 10.0;
				gl.xadvance = positions[i].x_advance / 10.0;
				gl.yadvance = positions[i].y_advance / 10.0;

				if (m_story.hasMark(firstChar))
				{
					GlyphLayout control;
					control.glyph = SpecialChars::OBJECT.unicode() + ScFace::CONTROL_GLYPHS;
					run.glyphs().append(control);
				}

				if (SpecialChars::isExpandingSpace(ch))
					gl.xadvance *= run.style().wordTracking();

				if (m_story.hasObject(firstChar))
					gl.xadvance = m_story.object(firstChar)->width() + m_story.object(firstChar)->lineWidth();

				double tracking = 0;
				if (flags & ScLayout_StartOfLine)
					tracking = style.fontSize() * style.tracking() / 10000.0;
				gl.xoffset += tracking;

				gl.scaleH = charStyle.scaleH() / 1000.0;
				gl.scaleV = charStyle.scaleV() / 1000.0;


				if (effects != ScStyle_Default)
				{
					double asce = style.font().ascent(style.fontSize() / 10.0);
					if (effects & ScStyle_Superscript)
					{
						gl.yoffset -= asce * m_item->doc()->typographicPrefs().valueSuperScript / 100.0;
						gl.scaleV = gl.scaleH = qMax(m_item->doc()->typographicPrefs().scalingSuperScript / 100.0, 10.0 / style.fontSize());
					}
					else if (effects & ScStyle_Subscript)
					{
						gl.yoffset += asce * m_item->doc()->typographicPrefs().valueSubScript / 100.0;
						gl.scaleV = gl.scaleH = qMax(m_item->doc()->typographicPrefs().scalingSubScript / 100.0, 10.0 / style.fontSize());
					}
					else
					{
						gl.scaleV = gl.scaleH = 1.0;
					}

					gl.scaleH *= style.scaleH() / 1000.0;
					gl.scaleV *= style.scaleV() / 1000.0;

					if (effects & ScStyle_SmallCaps)
					{
						double smallcapsScale = m_item->doc()->typographicPrefs().valueSmallCaps / 100.0;
						// FIXME HOST: This is completely wrong, we shouldn’t be changing
						// the glyph ids after the shaping!
						QChar uc = ch.toUpper();
						if (uc != ch)
						{
							gl.glyph = style.font().char2CMap(uc);
							gl.xadvance = style.font().glyphWidth(gl.glyph, style.fontSize() / 10);
							gl.scaleV *= smallcapsScale;
							gl.scaleH *= smallcapsScale;
						}
					}
				}

				if (gl.xadvance > 0)
					gl.xadvance += tracking;

				run.glyphs().append(gl);

				i++;
			}
			glyphRuns.append(run);
		}
	}

	return glyphRuns;
}
