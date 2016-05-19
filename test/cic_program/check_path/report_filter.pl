#!/usr/bin/perl 
####################################################################
##	     PrimeTime Function for EDA Contest 2016		  ##
####################################################################
#PrimeTime Report Filter
#
#Function
##Read the pt timing report file and kill same path
##Report number of paths and path with out cell delay info.
#
#Author
##CIC DSD DTS
##C.L. Huang
#
#Version Info.
##v20101112-1
###Init. veriosn
##v20101115-1
###Renaming title
###Write out the set false path cmd
###Write out the path info with out delay
####################################################################

####################################used only once:
## Usage
####################################
sub print_usage{

print <<ENDL;

        Usage : report_filter.pl <input file>

ENDL
}

####################################
## ARGV Check
####################################
if ($#ARGV == -1) {
    print_usage();
    exit;	
}
unless($#ARGV!=1){
    print_usage();
    exit;
}

####################################
## VARs Init.
####################################
$start_path = 0;
$end_path = 0;
$path_index = 1;
$dir_name = "report_filter";

####################################
## Main
####################################
##Create DIR
#print "\nSTEP: Create output DIR $dir_name and needed file.\n";
#system "perl make_dir.pl $dir_name" || die "Error: Fail to create the $dir_name DIR.\n";
#system "/bin/touch $dir_name/set_false_cmd.script" || die "Error: Fail to create the $dir_name/set_false_cmd.script to write\n";
#system "/bin/touch $dir_name/path.log" || die "Error: Fail to create the $dir_name/path.log to write.\n";

##Read the input file
#print "\nSTEP: Read the input file.\n";

open (INPUTFILE, "<$ARGV[0]") || die "Error: Fail to open $ARGV[0].\n";

while (<INPUTFILE>){
	chomp($_);
	#s/^\s+|\t//g;
	#replace tab to space
	s/\t/ /g;
	#delete the space in the head of line, ^means head
	s/^\s+//g;
	if(!($_=~/\S+/)) {next;}

	if ($_=~/\(in\)/) {
		$start_path = 1;
		$path = "";
		$false_cmd = "set_false_path \\\n";
	}
	
	if ($_=~/\(out\)/){
		if ($start_path == 1) {
			$end_path = 1;
		}
	}

	&process_path;
	
	if ($end_path == 1) {
		$start_path = 0;
		$end_path = 0;
		$path_index = $path_index + 1;
		$path = $path . " " .$configitem[3];
		$path_array{$path} = "";

		##Write out 
		&write_set_false_cmd;
		&write_path;

		##Print out
#		print "$path\n\n\n";
#		print "$false_cmd\n\n\n";
	}
	
}

close (INPUTFILE);

##Print path count
#print "\nSTEP: END of process the path report file.\n";
@path_keys = keys %path_array;
$path_num = $#path_keys + 1;
$path_index = $path_index - 1;
#print "Path summary:\n";
#print "Read $path_index paths, save $path_num paths\n";
print "$path_index $path_num\n";

####################################
## Sub
## Process path
####################################
sub process_path {
	if ($start_path == 1) {
		@configitem = split(/\s+/);
#		print "$configitem[0] $configitem[3] $configitem[4]\n";
		$path = $path . " " . $configitem[0] . " " .$configitem[4];
		if ($_=~/\(in\)/) {
			if ($configitem[4] eq "r") {
				$false_cmd = $false_cmd."-rise_from ".$configitem[0]." \\\n";
			} elsif ($configitem[4] eq "f") {
				$false_cmd = $false_cmd."-fall_from ".$configitem[0]." \\\n";
			} else {
				$false_cmd = $false_cmd."-from ".$configitem[0]." \\\n";
			}
		} elsif ($_=~/\(out\)/) {
                        if ($configitem[4] eq "r") {
                                $false_cmd = $false_cmd."-rise_to ".$configitem[0]." \n\n";
                        } elsif ($configitem[4] eq "f") {
                                $false_cmd = $false_cmd."-fall_to ".$configitem[0]." \n\n";
                        } else {
                                $false_cmd = $false_cmd."-to ".$configitem[0]." \n\n";
                        }
		} else {
                        if ($configitem[4] eq "r") {
                                $false_cmd = $false_cmd."-rise_through ".$configitem[0]." \\\n";
                        } elsif ($configitem[4] eq "f") {
                                $false_cmd = $false_cmd."-fall_through ".$configitem[0]." \\\n";
                        } else {
                                $false_cmd = $false_cmd."-through ".$configitem[0]." \\\n";
                        }
		}
	}
}

####################################
## Sub
## Write out set false cmd
####################################
sub write_set_false_cmd{

#open (WRITEFALSECMDFILE, ">>$dir_name/set_false_cmd.script") || die "Error: Can't open the $dir_name/set_false_cmd.script to write.\n";

#print WRITEFALSECMDFILE "$false_cmd\n\n\n";

close (WRITEFALSECMDFILE)
}

####################################
## Sub
## Write out path
####################################
sub write_path{

#open (WRITEPAHTFILE, ">>$dir_name/path.log") || die "Error: Cant' open the $dir_name/path.log to write.\n";

#print WRITEPAHTFILE "$path\n\n\n";

close (WRITEPAHTFILE);
}
