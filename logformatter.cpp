#include "logformatter.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QRegularExpression>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace {

QString decodeText(const QByteArray &raw)
{
    // Docker logs are usually UTF-8, but application logs sometimes emit
    // Latin-1 (or otherwise non-UTF-8) bytes; fall back rather than letting
    // fromUtf8 turn every invalid byte into a U+FFFD replacement glyph.
    if (raw.isValidUtf8())
        return QString::fromUtf8(raw);
    return QString::fromLatin1(raw);
}

QString stripAnsiAndControlChars(const QString &text)
{
    static const QRegularExpression ansiEscape(QStringLiteral("\x1B\\[[0-9;]*[a-zA-Z]"));

    QString cleaned = text;
    cleaned.remove(ansiEscape);

    QString result;
    result.reserve(cleaned.size());
    for (const QChar ch : cleaned) {
        const ushort code = ch.unicode();
        const bool isControl = code < 0x20 || code == 0x7F;
        const bool isAllowedWhitespace = ch == QLatin1Char('\t');
        if (!isControl || isAllowedWhitespace)
            result.append(ch);
    }
    return result;
}

QString tryFormatJson(const QString &line)
{
    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError || (!doc.isObject() && !doc.isArray()))
        return QString();
    return QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
}

QString tryFormatXml(const QString &line)
{
    QXmlStreamReader reader(line);
    QString output;
    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    bool sawElement = false;
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.hasError())
            return QString();
        if (reader.isStartElement() || reader.isEndElement())
            sawElement = true;
        if (reader.tokenType() != QXmlStreamReader::Invalid
            && reader.tokenType() != QXmlStreamReader::NoToken) {
            writer.writeCurrentToken(reader);
        }
    }

    if (reader.hasError() || !sawElement)
        return QString();
    return output;
}

} // namespace

QString LogFormatter::formatLine(const QByteArray &rawLine)
{
    const QString decoded = stripAnsiAndControlChars(decodeText(rawLine));
    const QString trimmed = decoded.trimmed();

    if (trimmed.isEmpty())
        return decoded;

    if (trimmed.startsWith(QLatin1Char('{')) || trimmed.startsWith(QLatin1Char('['))) {
        const QString asJson = tryFormatJson(trimmed);
        if (!asJson.isEmpty())
            return asJson;
    }

    if (trimmed.startsWith(QLatin1Char('<'))) {
        const QString asXml = tryFormatXml(trimmed);
        if (!asXml.isEmpty())
            return asXml;
    }

    return decoded;
}
