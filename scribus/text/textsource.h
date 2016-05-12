

class TextSource
{
	int length
	int StoryText::indexOf(const QString &str, int from, Qt::CaseSensitivity cs) const
int StoryText::indexOf(QChar ch, int from, Qt::CaseSensitivity cs) const

void StoryText::insertParSep(int pos)
void StoryText::removeParSep(int pos)
void StoryText::removeChars(int pos, uint len)
void StoryText::trim()
void StoryText::insertChars(int pos, QString txt, bool applyNeighbourStyle)
void StoryText::insertCharsWithSoftHyphens(int pos, QString txt, bool applyNeighbourStyle)
void StoryText::replaceChar(int pos, QChar ch)
void StoryText::insertObject(int ob)
void StoryText::insertObject(int pos, int ob)

void StoryText::replaceObject(int pos, int ob)
QString StoryText::plainText() const
QChar StoryText::text(int pos) const
String StoryText::text(int pos, uint len) const
QString StoryText::textWithSoftHyphens(int pos, uint len) const

bool StoryText::isHighSurrogate(int pos) const
bool StoryText::isLowSurrogate(int pos) const
LayoutFlags StoryText::flags(int pos) const

const CharStyle & StoryText::charStyle(int pos) const
const ParagraphStyle & StoryText::paragraphStyle(int pos) const

void StoryText::setDefaultStyle(const ParagraphStyle& style)

void StoryText::applyCharStyle(int pos, uint len, const CharStyle& style )

void StoryText::eraseCharStyle(int pos, uint len, const CharStyle& style )
void StoryText::applyStyle(int pos, const ParagraphStyle& style, bool rmDirectFormatting)

void StoryText::eraseStyle(int pos, const ParagraphStyle& style)
void StoryText::setStyle(int pos, const ParagraphStyle& style)
void StoryText::setCharStyle(int pos, uint len, const CharStyle& style)

void StoryText::getNamedResources(ResourceCollection& lists) const

void StoryText::replaceNamedResources(ResourceCollection& newNames)



