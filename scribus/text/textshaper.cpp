#include "textshaper.h"

TextShaper::TextShaper()
{

}

QList<TextShaper::TextRun> TextShaper::itemizeBiDi(QString &text)
{
	QList<TextRun> textRuns;
	UBiDi *obj = ubidi_open();
	UErrorCode err = U_ZERO_ERROR;

	UBiDiLevel parLevel = UBIDI_LTR;
	ParagraphStyle style = itemText.paragraphStyle(firstInFrame());
	if (style.direction() == ParagraphStyle::RTL)
		parLevel = UBIDI_RTL;

	ubidi_setPara(obj, text.utf16(), -1, parLevel, NULL, &err);
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

QList<TextShaper::TextRun> TextShaper::itemizeStyles(QList<TextRun> &runs, QMap<int, int> &textMap)
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
				CharStyle startstyle = itemText.charStyle(textMap.value(start));
				CharStyle endstyle = itemText.charStyle(textMap.value(end));
				if (!startstyle.equalForShaping(endstyle))
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

QList<TextShaper::TextRun> TextShaper::itemizeScript(QList<TextRun> &bidiRuns, QString &text)
{
	QList<TextRun> runs;
	ScriptRun scriptrun (text.utf16(), text.length());

	foreach (TextRun bidirun, bidiRuns)
	{
		int start = bidirun.start;
		QList<TextRun> subruns;
		while (scriptrun.next())
		{
			if (scriptrun.getScriptStart() <= start && scriptrun.getScriptEnd() > start)
				break;
		}

		while (start < bidirun.start + bidirun.len)
		{
			int end = qMin(scriptrun.getScriptEnd(), bidirun.start + bidirun.len);
			UScriptCode script = scriptrun.getScriptCode();
			if (bidirun.dir == UBIDI_RTL)
				subruns.prepend(TextRun(start, end - start, bidirun.dir, script));
			else
				subruns.append(TextRun(start, end - start, bidirun.dir, script));

			start = end;
			scriptrun.next();
		}

		scriptrun.reset();
		runs.append(subruns);
	}

	return runs;
}

QList<GlyphRun> TextShaper::shapeText()
{
	// maps expanded characters to itemText tokens.
	QMap<int, int> textMap;
	QString text;
	QList<GlyphRun> glyphRuns;
	for (int i = firstInFrame(); i < itemText.length(); ++i)
	{
		CharStyle currStyle(itemText.charStyle(i));

		Mark* mark = itemText.mark(i);
		if (itemText.hasMark(i))
		{
			mark->OwnPage = OwnPage;
			//itemPtr and itemName set to this frame only if mark type is different than MARK2ItemType
			if (!mark->isType(MARK2ItemType))
			{
				mark->setItemPtr(this);
				mark->setItemName(itemName());
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
				mark->setItemPtr(this);
				NotesStyle* nStyle = note->notesStyle();
				Q_ASSERT(nStyle != NULL);
				QString chsName = nStyle->marksChStyle();
				if (!chsName.isEmpty())
				{
					CharStyle marksStyle(m_Doc->charStyle(chsName));
					if (!currStyle.equiv(marksStyle))
					{
						currStyle.setParent(chsName);
						itemText.applyCharStyle(i, 1, currStyle);
					}
				}

				StyleFlag s(itemText.charStyle(i).effects());
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
				if (s != itemText.charStyle(i).effects())
				{
					CharStyle haveSuperscript;
					haveSuperscript.setFeatures(s.featureList());
					itemText.applyCharStyle(i, 1, haveSuperscript);
				}
			}
		}

		bool bullet = false;
		if (i == 0 || itemText.text(i - 1) == SpecialChars::PARSEP)
		{
			ParagraphStyle style = itemText.paragraphStyle(i);
			if (style.hasBullet() || style.hasNum())
			{
				bullet = true;
				if (mark == NULL || !mark->isType(MARKBullNumType))
				{
					itemText.insertMark(new BulNumMark(), i);
					i--;
					continue;
				}
				if (style.hasBullet())
					mark->setString(style.bulletStr());
				else if (style.hasNum() && mark->getString().isEmpty())
				{
					mark->setString("?");
					m_Doc->flag_Renumber = true;
				}
			}
		}

		if (!bullet && mark && mark->isType(MARKBullNumType))
		{
			itemText.removeChars(i, 1);
			i--;
			continue;
		}

		QString str = ExpandToken(i);
		if (str.isEmpty())
			str = SpecialChars::ZWNBSPACE;

		int effects = currStyle.effects() & ScStyle_UserStyles;
		if (effects & ScStyle_AllCaps)
			str = str.toUpper();

		for (int j = 0; j < str.length(); j++)
			textMap.insert(text.length() + j, i);

		text.append(str);
	}

	QList<TextRun> bidiRuns = itemizeBiDi(text);
	QList<TextRun> scriptRuns = itemizeScript(bidiRuns, text);
	QList<TextRun> textRuns = itemizeStyles(scriptRuns, textMap);


	QVector<uint> ucs4 = text.toUcs4();
	foreach (TextRun textRun, textRuns) {
		CharStyle cs = itemText.charStyle(textMap.value(textRun.start));
		int effects = cs.effects() & ScStyle_UserStyles;

		FT_Set_Char_Size(cs.font().ftFace(), cs.fontSize(), 0, 72, 0);
		QString lang = cs.language();
		hb_language_t language = hb_language_from_string (lang.toStdString().c_str(), lang.toStdString().length());

		// TODO: move to ScFace
		hb_font_t *hbFont = hb_ft_font_create_referenced(cs.font().ftFace());
		hb_buffer_t *hbBuffer = hb_buffer_create();
		hb_buffer_set_language(hbBuffer, language);

		hb_buffer_clear_contents(hbBuffer);
		hb_buffer_add_utf32(hbBuffer, ucs4.data(), ucs4.length(), textRun.start, textRun.len);
		hb_buffer_set_direction(hbBuffer, textRun.dir == UBIDI_LTR ? HB_DIRECTION_LTR : HB_DIRECTION_RTL);
		hb_buffer_set_script(hbBuffer, hb_icu_script_to_script(textRun.script));

		hb_shape(hbFont, hbBuffer, NULL, 0);

		unsigned int count = hb_buffer_get_length(hbBuffer);
		hb_glyph_info_t *glyphs = hb_buffer_get_glyph_infos(hbBuffer, NULL);
		hb_glyph_position_t *positions = hb_buffer_get_glyph_positions(hbBuffer, NULL);

		glyphRuns.reserve(count);
		for (size_t i = 0; i < count; i++)
		{
			uint32_t firstCluster = glyphs[i].cluster;
//			uint32_t nextCluster = firstCluster;
//			for (size_t j = i + 1; j < count && nextCluster == firstCluster; j++)
//				nextCluster = glyphs[j].cluster;
//			if (nextCluster == firstCluster)
//				nextCluster = textRun.start + textRun.len;

			assert(textMap.contains(firstCluster));
//			assert(textMap.contains(nextCluster - 1));
			int firstChar = textMap.value(firstCluster);
			int lastChar = firstChar;
//			int lastChar = textMap.value(nextCluster - 1);

			QChar ch = itemText.text(firstChar);
			LayoutFlags flags = itemText.flags(firstChar);
			const CharStyle& charStyle(itemText.charStyle(firstChar));

			GlyphRun run(&charStyle, flags, firstChar, lastChar, itemText.object(firstChar), textRun.dir == UBIDI_RTL, glyphRuns.length());
			if (SpecialChars::isExpandingSpace(ch))
				run.setFlag(ScLayout_ExpandingSpace);

			GlyphLayout gl;
			gl.glyph = glyphs[i].codepoint;
			// indirect way to call ScFace::emulateGlyph() as it is private.
			if (gl.glyph == 0)
				gl.glyph = cs.font().char2CMap(ch);
			gl.xoffset = positions[i].x_offset / 10.0;
			gl.yoffset = -positions[i].y_offset / 10.0;
			gl.xadvance = positions[i].x_advance / 10.0;
			gl.yadvance = positions[i].y_advance / 10.0;

			if (itemText.hasMark(firstChar))
			{
				GlyphLayout control;
				control.glyph = SpecialChars::OBJECT.unicode() + ScFace::CONTROL_GLYPHS;
				run.glyphs().append(control);
			}

			if (SpecialChars::isExpandingSpace(ch))
				gl.xadvance *= run.style().wordTracking();

			if (itemText.hasObject(firstChar))
				gl.xadvance = run.width();

			double tracking = 0;
			if (flags & ScLayout_StartOfLine)
				tracking = charStyle.fontSize() * charStyle.tracking() / 10000.0;
			gl.xoffset += tracking;

			gl.scaleH = charStyle.scaleH() / 1000.0;
			gl.scaleV = charStyle.scaleV() / 1000.0;


			if (effects != ScStyle_Default)
			{
				double asce = cs.font().ascent(cs.fontSize() / 10.0);
				if (effects & ScStyle_Superscript)
				{
					gl.yoffset -= asce * m_Doc->typographicPrefs().valueSuperScript / 100.0;
					gl.scaleV = gl.scaleH = qMax(m_Doc->typographicPrefs().scalingSuperScript / 100.0, 10.0 / cs.fontSize());
				}
				else if (effects & ScStyle_Subscript)
				{
					gl.yoffset += asce * m_Doc->typographicPrefs().valueSubScript / 100.0;
					gl.scaleV = gl.scaleH = qMax(m_Doc->typographicPrefs().scalingSubScript / 100.0, 10.0 / cs.fontSize());
				}
				else
				{
					gl.scaleV = gl.scaleH = 1.0;
				}

				gl.scaleH *= cs.scaleH() / 1000.0;
				gl.scaleV *= cs.scaleV() / 1000.0;

				if (effects & ScStyle_SmallCaps)
				{
					double smallcapsScale = m_Doc->typographicPrefs().valueSmallCaps / 100.0;
					// FIXME HOST: This is completely wrong, we shouldn’t be changing
					// the glyph ids after the shaping!
					QChar uc = ch.toUpper();
					if (uc != ch)
					{
						gl.glyph = cs.font().char2CMap(uc);
						gl.xadvance = cs.font().glyphWidth(gl.glyph, cs.fontSize() / 10);
						gl.yadvance = cs.font().glyphBBox(gl.glyph, cs.fontSize() / 10).ascent;
						gl.scaleV *= smallcapsScale;
						gl.scaleH *= smallcapsScale;
					}
				}
			}

			if (gl.yadvance <= 0)
				gl.yadvance = charStyle.font().glyphBBox(gl.glyph, charStyle.fontSize() / 10).ascent * gl.scaleV;

			if (gl.xadvance > 0)
				gl.xadvance += tracking;

			run.glyphs().append(gl);
			glyphRuns.append(run);
		}
	}

	return glyphRuns;
}

