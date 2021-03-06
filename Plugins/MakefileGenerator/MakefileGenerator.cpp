//-----------------------------------------------------------------------------
// File: MakefileGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 02.09.2014
//
// Description:
// Makefile generator.
//-----------------------------------------------------------------------------

#include "MakefileGenerator.h"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QIODevice> 
#include <IPXACTmodels/fileset.h>

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::MakefileGenerator()
//-----------------------------------------------------------------------------
MakefileGenerator::MakefileGenerator( MakefileParser& parser, IPluginUtility* utility )
    : parsedData_( parser.getParsedData() ), generalFileSet_( parser.getGeneralFileSet() ),
    utility_( utility )
{
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::~MakefileGenerator()
//-----------------------------------------------------------------------------
MakefileGenerator::~MakefileGenerator()
{
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::generate()
//-----------------------------------------------------------------------------
void MakefileGenerator::generate(QString targetPath, QString topPath, QString sysViewName) const
{
    // Names of the created directories to be referenced by the master makefile.
    QStringList makeNames;
    // Create base directory for the software.
    QString basePath = targetPath + "/sw_" + sysViewName + "/";
    QDir path;
    path.mkpath( basePath );

    foreach(MakefileParser::MakeFileData mfd, parsedData_)
    {
        generateInstanceMakefile(basePath, topPath, mfd, makeNames);
    }

    generateMainMakefile(basePath, topPath, makeNames);

    generateLauncher(basePath, topPath, makeNames);
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::generateInstanceMakefile()
//-----------------------------------------------------------------------------
void MakefileGenerator::generateInstanceMakefile(QString basePath, QString topPath,
    MakefileParser::MakeFileData &mfd, QStringList &makeNames) const
{
    // Construct the path and folder.
    QString instancePath = basePath + mfd.name;
    QDir path;
    path.mkpath( instancePath );

    // Create the makefile and open a stream to write it.
    QString makePath = instancePath + "/Makefile"; 
    QFile makeFile(makePath);
    
    if ( !makeFile.open(QIODevice::WriteOnly) )
    {
        utility_->printError("Could not open the makefile at location " + makePath);
        utility_->printError("Reason: " + makeFile.errorString() );
    }

    QTextStream outStream(&makeFile);

    // Add the path to the list of created makefiles for later reference.
    makeNames.append( instancePath );

    // Write the paths of directories containing includes.
    outStream << "_INCLUDES=";

    foreach( QString path, mfd.includeDirectories )
    {
        outStream << " " << General::getRelativePath(instancePath,path);
    }

    outStream << endl;
	// Will make the -I option out of every included directory.
    outStream << "INCLUDES=$(patsubst %, -I%, $(_INCLUDES))" << endl << endl;

    // The include files themselves are dependencies of the source files.
    outStream << "DEPS=";

    foreach(QSharedPointer<MakefileParser::MakeObjectData> file, mfd.includeFiles)
    {
        outStream << " " << General::getRelativePath(instancePath,file->path) << "/" << file->fileName;
    }

    outStream << endl << endl;

    // Other stuff is in their own functions.
    writeFinalFlagsAndBuilder(mfd, outStream);

    writeObjectList(mfd, outStream);
    writeExeBuild(outStream);

	// Create rule for using debugging and profiling options
	outStream << "DEBUG_FLAGS +=" << endl;
	outStream << "debug: DEBUG_FLAGS += -ggdb" << endl;
	outStream << "debug: $(ENAME)" << endl << endl;

	outStream << "PROFILE_FLAGS +="<< endl;
	outStream << "profile: PROFILE_FLAGS += -pg -fno-omit-frame-pointer -fno-inline-functions "
		"-fno-inline-functions-called-once -fno-optimize-sibling-calls" << endl;
	outStream << "profile: $(ENAME)" << endl;

    writeMakeObjects(mfd, outStream, mfd.swObjects, instancePath);
    writeMakeObjects(mfd, outStream, mfd.hwObjects, instancePath);

    // Close after it is done.
    makeFile.close();

    QSharedPointer<FileSet> fileSet = mfd.fileSet;

    // The path in the fileSet must be relative to the basePath.
    QString relDir = General::getRelativePath(topPath,makePath);

    // Add the file to instance fileSet
    if ( !fileSet->contains(relDir) )
    {
        QSharedPointer<File> file;
        QStringList types;
        types.append("makefile");
        QSettings settings;
        file = fileSet->addFile(relDir, settings);
        file->setAllFileTypes( types );
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::generateMainMakefile()
//-----------------------------------------------------------------------------
void MakefileGenerator::generateMainMakefile(QString basePath, QString topPath, QStringList makeNames) const
{
    QString dir = basePath + "Makefile";
    // Create the master makefile.
    QFile makeFile( basePath + "Makefile" );
    makeFile.open(QIODevice::WriteOnly);
    QTextStream outStream(&makeFile);

    // Default target for each directory.
    outStream << "make:";

    foreach( QString directory, makeNames )
    {
        outStream << endl << "\t(cd " << General::getRelativePath(basePath,directory) << "; make)";
    }

    // Needs also cleaner for each directory.
    outStream << endl << endl << "clean:";

    foreach( QString directory, makeNames )
    {
        outStream << endl << "\t(cd " << General::getRelativePath(basePath,directory) << "; make clean)";
	}

	// Debug target for each directory.
	outStream << endl << endl << "debug:";

	foreach( QString directory, makeNames )
	{
		outStream << endl << "\t(cd " << General::getRelativePath(basePath,directory) << "; make debug)";
	}

	// Profiling target for each directory.
	outStream << endl << endl << "profile:";

	foreach( QString directory, makeNames )
	{
		outStream << endl << "\t(cd " << General::getRelativePath(basePath,directory) << "; make profile)";
	}

    // Close after it is done.
    makeFile.close();

    // The path in the fileSet must be relative to the basePath.
    QString relDir = General::getRelativePath(topPath,dir);

    // Add the file to instance fileSet
    if ( !generalFileSet_->contains(relDir) )
    {
        QSharedPointer<File> file;
        QStringList types;
        types.append("makefile");
        QSettings settings;
        file = generalFileSet_->addFile(relDir, settings);
        file->setAllFileTypes( types );
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::generateLauncher()
//-----------------------------------------------------------------------------
void MakefileGenerator::generateLauncher(QString basePath, QString topPath, QStringList makeNames) const
{
    QString dir = basePath + "launcher.sh";
    QFile launcherFile( dir );
    launcherFile.open(QIODevice::WriteOnly);
    QTextStream outStream(&launcherFile);

    writeProcessList(outStream, makeNames, basePath);

    writeShellFunctions(outStream);

    writeProcessLaunch(outStream);

    writeEnding(outStream);

    // Close after it is done.
    launcherFile.close();

    // The path in the fileSet must be relative to the basePath.
    QString relDir = General::getRelativePath(topPath,dir);

    // Add the file to instance fileSet
    if ( !generalFileSet_->contains(relDir) )
    {
        QSharedPointer<File> file;
        QStringList types;
        types.append("shellScript");
        QSettings settings;
        file = generalFileSet_->addFile(relDir, settings);
        file->setAllFileTypes( types );
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeFinalFlagsAndBuilder()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeFinalFlagsAndBuilder(MakefileParser::MakeFileData &mfd, QTextStream& outStream) const
{
    QString finalFlags = "$(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS)";
    QString finalBuilder;

    // If build command of software view of the hardware instance exist, its properties are used.
    if ( mfd.hwBuildCmd != 0 )
    {
        finalBuilder = mfd.hwBuildCmd->getCommand();

        finalFlags += " " + mfd.hwBuildCmd->getFlags();
    }

    // All flags of all software views must be appended to the flags.
    foreach ( QString flag, mfd.softViewFlags )
    {
        finalFlags += " " + flag;
    }

    // Finally, write down what we learned.
    outStream << "ENAME= " << mfd.name << endl;
    outStream << "EFLAGS= " << finalFlags << endl;
    outStream << "EBUILDER= " << finalBuilder << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeObjectList()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeObjectList(MakefileParser::MakeFileData &mfd, QTextStream& outStream) const
{
    // Since hardware and software fileSets are different entities, both have to be looped through.
    // Include files are skipped and the object file is simply original filename + ".o".
    outStream << "_OBJ=";

    foreach(QSharedPointer<MakefileParser::MakeObjectData> mod, mfd.swObjects)
    {
        if ( !mod->file->getIncludeFile() && !mod->compiler.isEmpty() )
        {
            outStream << " " << mod->fileName << ".o";
        }
    }

    foreach(QSharedPointer<MakefileParser::MakeObjectData> mod, mfd.hwObjects)
    {
        if ( !mod->file->getIncludeFile() && !mod->compiler.isEmpty() )
        {
            outStream << " " << mod->file->getName() << ".o";
        }
    }

    // Finally, write down what we learned.
    outStream << endl;
    outStream << "ODIR= obj" << endl;
    outStream << "OBJ= $(patsubst %,$(ODIR)/%,$(_OBJ))" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeExeBuild()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeExeBuild(QTextStream& outStream) const
{
    // Rather straight forward: write constant build rule and a cleaner rule.
    outStream << "$(ENAME): $(OBJ)" << endl;
    outStream << "\t$(EBUILDER) -o $(ENAME) $(OBJ) $(EFLAGS)"
        << endl << endl;

    // Delete all known object files. May leave renamed files undeleted, but is more secure than deleting all
    // content of the object directory.
    outStream << "clean:\n\trm -f $(OBJ);" << endl << endl;

    // Make a directory for the object files.
    outStream << "all: $(OBJ)" << endl << endl;
    outStream << "$(OBJ): | $(ODIR)" << endl << endl;
    outStream << "$(ODIR):\n\tmkdir -p $(ODIR)" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeMakeObjects()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeMakeObjects(MakefileParser::MakeFileData &mfd, QTextStream& outStream, QList<QSharedPointer<MakefileParser::MakeObjectData> >& objects, QString instancePath) const
{
    foreach(QSharedPointer<MakefileParser::MakeObjectData> mod, objects)
    {
        // Skip the include files. Those do not need their own object files.
        if ( mod->file->getIncludeFile() || mod->compiler.isEmpty() )
        {
            continue;
        }

        // Flags will always include at least the includes.
        QString cFlags = "$(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) ";

        // The other flags are not hard coded.
        cFlags += mod->fileBuildCmd->getFlags();
        cFlags += getFileFlags(mod, mfd);

        // The relative path is needed by the make and the builder to access the source file.
        QString relPath = General::getRelativePath(instancePath,mod->path);
        QString fileName = mod->fileName;

        // Write the rule for building the individual object file, including dependencies.
        outStream << endl;
        outStream << "$(ODIR)/" << fileName << ".o: $(DEPS) " << relPath << "/" << fileName << endl;
        outStream << "\t" << mod->compiler << " -c -o $(ODIR)/" << fileName << ".o " <<
            relPath << "/" << fileName << " " << cFlags << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::getFileFlags()
//-----------------------------------------------------------------------------
QString MakefileGenerator::getFileFlags(QSharedPointer<MakefileParser::MakeObjectData> &mod, MakefileParser::MakeFileData &mfd) const
{
    QString cFlags;

    // This mesh does following:
    // 1. If file does not override flags, may use fileSet flags
    // 2. If fileSet does not override flags, may use software flags
    // 2. If software does not override flags, may use hardware flags
    if ( !mod->fileBuildCmd->getReplaceDefaultFlags() )
    {
        if ( mod->fileSetBuildCmd != 0 )
        {
            cFlags += " " + mod->fileSetBuildCmd->getFlags();
        }

        if ( mod->fileSetBuildCmd == 0 || !mod->fileSetBuildCmd->getReplaceDefaultFlags() )
        {
            if ( mod->swBuildCmd != 0 )
            {
                cFlags += " " + mod->swBuildCmd->getFlags();
            }   

            if ( ( mod->swBuildCmd == 0 || !mod->swBuildCmd->getReplaceDefaultFlags() ) && mfd.hwBuildCmd != 0 )
            {
                cFlags += " " + mfd.hwBuildCmd->getFlags();
            }
        }
    }

    return cFlags;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeProcessList()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeProcessList(QTextStream& outStream, QStringList makeNames, QString basePath) const
{
    outStream << "PROCESSES=(";

    // The list consists of relative path to each executable.
    foreach( QString directory, makeNames )
    {
        QFileInfo qfi = QFileInfo( directory );

        outStream << General::getRelativePath(basePath,directory) << "/" << qfi.fileName() << " ";
    }

    outStream << ")" << endl;
    outStream << "echo ${PROCESSES[@]}" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeShellFunctions()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeShellFunctions(QTextStream& outStream) const
{
    // Function called from the signal trap to first kill all processes, then quit the script.
    outStream << "terminate () {" << endl;
    outStream << "\tcommit_hits" << endl;
    outStream << "\texit" << endl;
    outStream << "}" << endl << endl;

    // Function killing all processes, if none exists.
    outStream << "commit_hits () {" << endl;
    outStream << "\thitlist=hitlist.txt" << endl << endl;

    // Check first there is existing hit list.
    outStream << "\tif [ -f $hitlist ]; then" << endl;

    // If does, loop through it and kill by name.
    outStream << "\t\techo \"KILLING PROCESSES\"" << endl << endl;
    outStream << "\t\twhile read line" << endl;
    outStream << "\t\tdo" << endl;
    outStream << "\t\t\tkillall $line &> /dev/null" << endl;
    outStream << "\t\tdone < $hitlist" << endl;

    outStream << "\tfi" << endl;
    outStream << "}" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeProcessLaunch()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeProcessLaunch(QTextStream& outStream) const
{
    // Before launch proper, kill pre existing processes and remove the hit list.
    outStream << "commit_hits" << endl;
    outStream << "rm $hitlist &> /dev/null" << endl << endl;

    outStream << "echo \"LAUNCHING NEW PROCESSES\"" << endl;

    // Loop throug the list of processes.
    outStream << "for name in \"${PROCESSES[@]}\"" << endl << "do" << endl;
        // Launch it and record the pid for later use.
        outStream << "\t./$name &" << endl;
        outStream << "\tmypid=$!" << endl;
        outStream << "\tpids=$pids\"$mypid \"" << endl;

        // Record the name to hit list.
        outStream << "\techo \"$name\" >> $hitlist" << endl;
    outStream << "done" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeEnding()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeEnding(QTextStream& outStream) const
{
    // Signal trap to called when calling suspend process.
    outStream << "trap \"terminate\" SIGTSTP" << endl;

    // Wait for the processes to finish before exiting the script execution.
    outStream << "echo \"PROCESSES LAUNCHED\"" << endl;
    outStream << "wait $pids" << endl << endl;

    outStream << "echo \"DONE\"" << endl;
}

