#ifndef LOGFORMATTER_H
#define LOGFORMATTER_H

#include <QByteArray>
#include <QString>

namespace LogFormatter {

// Decodes a single raw log line (no trailing newline), strips ANSI escape
// codes and non-printable control bytes, and pretty-prints the line if it
// is a complete JSON or XML document.
QString formatLine(const QByteArray &rawLine);

} // namespace LogFormatter

#endif // LOGFORMATTER_H
