#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>

static int otherFiles = 0;
static int codeFiles = 0;
static int headerFiles = 0;
static int resourceFiles = 0;

static int emptyLines = 0;
static int commentLines = 0;
static int preprocessorLines = 0;
static int codeLines = 0;
static bool recurse = false;

QTextStream out (stdout);

void processCode (const QString& path)
{
    QFile in (path);
    if (!in.open (QFile::ReadOnly))
    {
        qDebug ().nospace ().noquote () << "Unable to open file \"" << path << "\".";
        return;
    }

    auto lines = in.readAll ().split ('\n');
    for (auto& line : lines)
    {
        line = line.trimmed ();
        if (line.isEmpty ())
        {
            emptyLines++;
        }
        else
        {
            if (line.startsWith ("//") || line.startsWith ("/*"))
            {
                commentLines++;
            }
            else if (line.startsWith ("#"))
            {
                preprocessorLines++;
            }
            else
            {
                codeLines++;
            }
        }
    }
}

void processFile (const QFileInfo& entry)
{
    auto suffix = entry.suffix ();
    if (suffix == QStringLiteral ("cpp") || suffix == QStringLiteral ("c"))
    {
        codeFiles++;
        processCode (entry.absoluteFilePath ());
    }
    else if (suffix == QStringLiteral ("h"))
    {
        headerFiles++;
        processCode (entry.absoluteFilePath ());
    }
    else if (suffix == QStringLiteral ("qrc"))
    {
        resourceFiles++;
    }
    else
    {
        otherFiles++;
    }
}

bool processFolder (const QString& path)
{
    const QDir dir (path);
    if (!dir.exists ())
    {
        qDebug ().nospace ().noquote () << "No such folder \"" << path << "\".";
        return false;
    }

    if (path.endsWith (QStringLiteral ("/build")))
    {
        return false;
    }

    auto entries = dir.entryInfoList (QDir::Files | QDir::NoDotAndDotDot);
    for (const auto& entry : std::as_const (entries))
    {
        processFile (entry);
    }
    if (recurse)
    {
        entries = dir.entryInfoList (QDir::Dirs | QDir::NoDotAndDotDot);
        for (const auto& entry : std::as_const (entries))
        {
            processFolder (entry.absoluteFilePath ());
        }
    }
    return true;
}

int main (int argc, char* argv[])
{
    const QCoreApplication a (argc, argv);

    QCoreApplication::setApplicationName (QStringLiteral ("lineCounter"));
    QCoreApplication::setApplicationVersion (QStringLiteral (VERSION));
    QCoreApplication::setOrganizationName (QStringLiteral ("Netherwood Industries"));

    QCommandLineParser parser;
    parser.setApplicationDescription (QStringLiteral ("Calculate line metrics for C/C++ code."));
    parser.addHelpOption ();
    parser.addVersionOption ();
    parser.addPositionalArgument (QStringLiteral ("root"), QStringLiteral ("Path to root of code project"));
    parser.addOption (QCommandLineOption (QStringList () << QByteArrayLiteral ("recurse") << QByteArrayLiteral ("r"), QStringLiteral ("Recurse into sub directories")));
    parser.process (a);

    recurse = parser.isSet (QStringLiteral ("recurse"));

    if (parser.positionalArguments ().isEmpty())
    {
        parser.showHelp (100);
    }

    QFileInfo info (parser.positionalArguments ().constFirst ());
    bool showFileInfo = true;
    if (info.exists() && info.isFile())
    {
        processFile (info);
        showFileInfo = false;
    }

    if (processFolder (parser.positionalArguments ().constFirst ()))
    {
        const QLocale locale;

        if (showFileInfo)
        {
            out << "Files: " << '\n';
            out << "  Code:         " << locale.toString (codeFiles) << '\n';
            out << "  Header:       " << locale.toString (headerFiles) << '\n';
            out << "  Resource:     " << locale.toString (resourceFiles) << '\n';
            out << "  Other:        " << locale.toString (otherFiles) << '\n';
            out << "  TOTAL:        " << locale.toString (codeFiles + headerFiles + resourceFiles + otherFiles) << "\n\n";
        }

        out << "Lines: " << '\n';
        out << "  Code:         " << locale.toString (codeLines) << '\n';
        out << "  Comment:      " << locale.toString (commentLines) << '\n';
        out << "  Preprocessor: " << locale.toString (preprocessorLines) << '\n';
        out << "  Empty:        " << locale.toString (emptyLines) << '\n';
        out << "  TOTAL         " << locale.toString (codeLines + commentLines + preprocessorLines + emptyLines) << '\n';
        return 0;
    }
    return 100;
}
