#!/usr/bin/perl 
####################################################################
##           PrimeTime Function for EDA Contest 2016              ##
####################################################################
#Write out the PrimeTime cmd: report_timing -justify
#
#Function
##Reading the timing report and set the path to justify 
##Write out the PrimeTime cmd
##The PrimeTime cmd will exec paralle in PrimeTime
#
#Author
##CIC DSD DTS
##C.L. Huang
#
#Version Info.
##v20101113-1
###Init. veriosn
####################################################################

####################################
## Usage        
####################################
sub print_usage{

print <<ENDL;   
        
        Usage : compare1.pl <filename> 
               
ENDL
}

####################################
## ARGV Check
####################################
if ($#ARGV == -1) {
    print_usage();
    exit;
}
unless($#ARGV!=2){
    print_usage();
    exit;
}

#################################### 
## VARs 
####################################
#$dir_name = $ARGV[0];
$rptfile_name = $ARGV[0];
$path_now_com =0;

#################################### 
## Main                         
####################################
#print "\nStart the perl function.\n";

#print "\nSTEP: chdir to $dir_name DIR.\n";
#chdir $dir_name || die "Error: Fail to chdir to $dir_name.\n"; 

print "**********************************************************************\n";
print "*Step 4. Extract the path info. from the result of running prime time*"."\n";
print "**********************************************************************\n";


#print "\nSTEP: Create file system.\n";
if (-e "../tmp/tmp2") {
	unlink "../tmp/tmp2";
}
system "/bin/touch ../tmp/tmp2" || die "Error: Fail to create ../tmp/tmp2\n";
#system "/usr/bin/perl ../make_dir.pl justify_db" || die "Error: Fail to create justify_db DIR.\n";

#print "\nSTEP: Read the report and write out script.\n";
&read_report;

#################################### 
## Sub
## Read the report lesser than file
####################################
sub read_report{
#print "Notice: Read the report lesser than file.\n";

$get_path = 0;
#$path_count = 0;
#$path_num = 0;

open (REPORTFILE, "<$rptfile_name") || die "Error: Can't find the $rptfile_name.\n";

while (<REPORTFILE>){
	chomp($_);
        #s/^\s+|\t//g;
	#replace tab to space
	s/\t/ /g;
	#delete the space in the head of line, ^means head
	s/^\s+//g;
        if(!($_=~/\S+/)) {next;}
        #$justify_cmd = "now is !!!!!!!!".$_."\n";
	#&write_cmd;
	
	
	
	
	if($_=~/path/i){
	if($_=~/([\d].*[^\}])/s){
	
	$justify_cmd = $1."\n";
	&write_cmd;
	$path_now_com = $1;
	}
	}
	
	if($_=~/No constrained paths./i){
	print "NOTE: Path $path_now_com is incorrect or not meet the Slack Constraint\n";
	}
	
	if ($_=~/\(out\)/) {
		#print "\nNotice: End fetch path.\n";
		$get_path = 0;
		@line = split(/\s+/);
		$justify_cmd = $line[0].$line[1].$line[2].$line[3].$line[4]."\n";
	        &write_cmd;
	}
	
	if ($get_path == 1){
	@line = split(/\s+/);
	$justify_cmd = $line[0].$line[1].$line[3].$line[4].$line[5]."\n";
	 &write_cmd;
	}
	
	
	if ($_=~/\(in\)/) {
		#print "\nNotice: Start fetch path.\n";
		$get_path = 1;
		@line = split(/\s+/);
	        $justify_cmd = $line[0].$line[1].$line[2].$line[3].$line[4]."\n";
	        &write_cmd;
	}
	
	if ($_=~/slack/i) {
	  if($_=~/slack_lesser_than/i){
	  }
	  else{
	  @line = split(/\s+/);
	  $justify_cmd = $line[0].$line[2]."\n";
	  &write_cmd;
	  }
	}
}
close(REPORTFILE);

}

#################################### 
## Sub
## Write parallel execute cmd
####################################
sub write_cmd{
#print "Notice: Write the cmd.\n";

open (WRITECMDFILE, ">>../tmp/tmp2") || die "Error: Can't open ../tmp/tmp2 to write.\n";

print WRITECMDFILE $justify_cmd;

close (WRITECMDFILE);

}
