#!/usr/bin/perl

# Copies binaries to Valve engined games and to a base folder
use Getopt::Std;
use File::Copy;
use File::Basename;
use File::Spec::Functions qw(rel2abs);

sub copy_binaries;
sub create_folder;

#Setup vars here
$BASE_FOLDER=dirname(rel2abs($0)) . "/..";
$CORE_BIN="mani_admin_plugin";
$LINUX_BASE=$BASE_FOLDER;
$WINDOWS_BASE=$BASE_FOLDER;
$ROOT_GAME="/srcds_1";
$ROOT_ORANGE_GAME="/srcds_1/orangebox";
$PLUGIN_BASE="VSP";

%option = ();
getopts("rso", \%option);

$SMM_EXT="";
if ($option{r})
{
	print "RELEASE MODE\n";
	$RELEASE="TRUE";
}

if ($option{s})
{
    print "MM:S Version\n";
    $SMM="TRUE";
    $SMM_EXT="_mm";
	$PLUGIN_BASE="SourceMM";
}

if ($option{o})
{
    print "ORANGE SDK\n";
    $ORANGE="TRUE"
}

if ($^O eq "MSWin32")
{
	print "Windows platform\n";
	$WINDOWS="TRUE";
	$DEV_BASE=$WINDOWS_BASE;
	$FILE_EXT=".dll";
	$SMM_FILE_EXT=".dll";
}
else
{
#Linux platform
	print "Linux platform\n";
	$DEV_BASE=$LINUX_BASE;
	$FILE_EXT="_i486.so";
	$SMM_FILE_EXT=".so";
}

$BIN_FOLDER=$DEV_BASE . "/plugin_output";

if ($ORANGE)
{
    $ENGINE_BASE=$DEV_BASE . $ROOT_ORANGE_GAME;
} 
else 
{
    $ENGINE_BASE=$DEV_BASE . $ROOT_GAME;
}


$BIN_FILE=$CORE_BIN . $SMM_EXT . $FILE_EXT;
$PDB_FILE=$CORE_BIN . $SMM_EXT . ".pdb";

print "INFO:\nDEV_BASE:  $DEV_BASE\nENGINE_BASE:  $ENGINE_BASE\nBIN_FOLDER:  $BIN_FOLDER\n";
print "File = " . $BIN_FILE . "\n";

opendir MYDIR, "$ENGINE_BASE/";
@contents = grep !/^\.\.?$/, readdir MYDIR;
closedir MYDIR;

foreach $listitem ( @contents )
{
	if ( -d $ENGINE_BASE . "/" . $listitem && $listitem ne "hl2")
	{
		if ( -e $ENGINE_BASE . "/" . $listitem . "/gameinfo.txt")
		{
			copy_binaries($listitem);	
		}
	}
}

#Copy ready for a build
if ($RELEASE)
{
	if ($ORANGE)
	{
		print "Copying $BIN_FOLDER/orange_bin/$PLUGIN_BASE/$BIN_FILE to $DEV_BASE/public_build/orange_bin/$BIN_FILE\n";
		copy ("$BIN_FOLDER/orange_bin/$PLUGIN_BASE/$BIN_FILE",
		"$DEV_BASE/public_build/orange_bin/$BIN_FILE");
		if ($^O eq "MSWin32")
		{
			print "Copying $BIN_FOLDER/orange_bin/$PLUGIN_BASE/$PDB_FILE to $DEV_BASE/public_build/orange_bin/$PDB_FILE\n";
			copy ("$BIN_FOLDER/orange_bin/$PLUGIN_BASE/$PDB_FILE",
			"$DEV_BASE/public_build/orange_bin/$PDB_FILE");
		}
	}
	else
	{
		print "Copying $BIN_FOLDER/legacy_bin/$PLUGIN_BASE/$BIN_FILE to $DEV_BASE/public_build/legacy_bin/$BIN_FILE\n";
		copy ("$BIN_FOLDER/legacy_bin/$PLUGIN_BASE/$BIN_FILE",
		"$DEV_BASE/public_build/legacy_bin/$BIN_FILE");
		if ($^O eq "MSWin32")
		{
			print "Copying $BIN_FOLDER/legacy_bin/$PLUGIN_BASE/$PDB_FILE to $DEV_BASE/public_build/legacy_bin/$PDB_FILE\n";
			copy ("$BIN_FOLDER/legacy_bin/$PLUGIN_BASE/$PDB_FILE",
			"$DEV_BASE/public_build/legacy_bin/$PDB_FILE");
		}
	}
}	
sleep(1);

#### Functions
sub copy_binaries
{
my $mod_dir = $ENGINE_BASE . "/" . $_[0];
my $search_curly = 0;

	print "Setting up binaries for " . $_[0] . "\n";
	create_folder("$mod_dir/addons");
	create_folder("$mod_dir/addons/mani_admin_plugin");
	create_folder("$mod_dir/addons/mani_admin_plugin/bin");
	create_folder("$mod_dir/addons/metamod/");
	create_folder("$mod_dir/addons/metamod/bin");
	create_folder("$mod_dir/cfg");
	create_folder("$mod_dir/cfg/mani_admin_plugin");

	#Copy Meta Mod Source 1.8 binary
	copy ("$DEV_BASE/sourcemm_bin/sourcemm_1_8/server" . $FILE_EXT,
		"$mod_dir/addons/metamod/bin/server" . $FILE_EXT);
	
	if ($ORANGE)
	{
		copy ("$DEV_BASE/sourcemm_bin/sourcemm_1_8/metamod.2.ep2" . $SMM_FILE_EXT,
			"$mod_dir/addons/metamod/bin/metamod.2.ep2" . $SMM_FILE_EXT);
		copy ("$DEV_BASE/sourcemm_bin/sourcemm_1_8/metamod.2.ep2v" . $SMM_FILE_EXT,
			"$mod_dir/addons/metamod/bin/metamod.2.ep2v" . $SMM_FILE_EXT);
	}
	else
	{
		copy ("$DEV_BASE/sourcemm_bin/sourcemm_1_8/metamod.1.ep1" . $SMM_FILE_EXT,
			"$mod_dir/addons/metamod/bin/metamod.1.ep1" . $SMM_FILE_EXT);
	}

	if ($ORANGE)
	{
		if ($SMM)
		{
			print "Copying $BIN_FOLDER/orange_bin/$PLUGIN_BASE/$BIN_FILE to $mod_dir/addons/mani_admin_plugin/bin/$BIN_FILE\n";
			copy ("$BIN_FOLDER/orange_bin/$PLUGIN_BASE/$BIN_FILE",
				"$mod_dir/addons/mani_admin_plugin/bin/$BIN_FILE");
		}
		else
		{
			print "Copying $BIN_FOLDER/orange_bin/$PLUGIN_BASE/$BIN_FILE to $mod_dir/addons/$BIN_FILE\n";
			copy ("$BIN_FOLDER/orange_bin/$PLUGIN_BASE/$BIN_FILE",
				"$mod_dir/addons/$BIN_FILE");
		}
	}
	else
	{
		if ($SMM)
		{
			print "Copying $BIN_FOLDER/legacy_bin/$PLUGIN_BASE/$BIN_FILE to $mod_dir/addons/mani_admin_plugin/bin/$BIN_FILE\n";
			copy ("$BIN_FOLDER/legacy_bin/$PLUGIN_BASE/$BIN_FILE",
				"$mod_dir/addons/mani_admin_plugin/bin/$BIN_FILE");
		}
		else
		{
			print "Copying $BIN_FOLDER/legacy_bin/$PLUGIN_BASE/$BIN_FILE to $mod_dir/addons/$BIN_FILE\n";
			copy ("$BIN_FOLDER/legacy_bin/$PLUGIN_BASE/$BIN_FILE",
				"$mod_dir/addons/$BIN_FILE");
		}
	}

	#Parse gameinfo.txt and setup for correct mode
	open(DAT, "$mod_dir/gameinfo.txt");
	@raw_data=<DAT>;
	close(DAT);

	if ($SMM)
	{	
		if (not grep(/metamod/, @raw_data))
		{
			open(DAT, ">$mod_dir/gameinfo.txt");

			$next_line = 0;
			foreach $LINE_VAR (@raw_data)
			{
				if (grep(/SearchPaths/, $LINE_VAR))
				{
					$search_curly = 1;
				}
				else
				{
					if ($search_curly ne 0)
					{
						if (grep(/{/, $LINE_VAR))
						{
							$search_curly = 1;
						}				
						else
						{
							$search_curly = 2;
						}
					}
				}
				

				if ($search_curly eq 2)
				{
					print DAT "                              GameBin                         |gameinfo_path|addons/metamod/bin\n";
					$search_curly = 0;
				}

				print DAT $LINE_VAR;
			}
			close(DAT);
		}

		unlink("$mod_dir/addons/mani_admin_plugin.vdf");
	}
	else
	{		
		if (grep(/metamod/, @raw_data))
		{
			open(DAT, ">$mod_dir/gameinfo.txt");
			foreach $LINE_VAR (@raw_data)
			{
				if (not grep(/metamod/, $LINE_VAR))
				{
					print DAT $LINE_VAR;
				}
			}
			close(DAT);
		}

		open(DAT, ">$mod_dir/addons/$CORE_BIN.vdf");
		print DAT "\"Plugin\"\n";
		print DAT "{\n";
		print DAT "\t\"file\" \"../$_[0]/addons/".$CORE_BIN."_i486\"\n";
		print DAT "}\n";
		close(DAT);
	}

}

sub create_folder
{
	if (not -d $mod_dir . $_[0])
	{
		print "Adding folder $mod_dir" . $_[0] . "\n";
		mkdir $mod_dir . $_[0];
	}
}
