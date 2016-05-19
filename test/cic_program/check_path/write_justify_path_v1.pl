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
##CIC DSD MSS
##C.L. Yang
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
        
        Usage : write_justify_path.pl <filename> <slack_constraint> <case number>
               
ENDL
}

####################################
## ARGV Check
####################################
if ($#ARGV == -1) {
    print_usage();
    exit;
}
unless($#ARGV!=3){
    print_usage();
    exit;
}

#################################### 
## VARs 
####################################
#$dir_name = $ARGV[0];
$rptfile_name = $ARGV[0];
$slack_less = $ARGV[1];
$case_num = $ARGV[2];
#print "case_num=$case_num\n";

print "**********************************************************************\n";
print "*Step 1. Convert the true path list file format to prime time command*"."\n";
print "**********************************************************************\n";

#################################### 
## Main                         
####################################
#print "\nStart the perl function.\n";

#print "\nSTEP: chdir to $dir_name DIR.\n";
#chdir $dir_name || die "Error: Fail to chdir to $dir_name.\n"; 

#print "\nSTEP: Create file system.\n";

#print "the result is !!!!!".$pwd_result;
if (-e "../tmp/parallel_execute_file") {
	unlink "../tmp/parallel_execute_file";
}
system "/bin/touch  ../tmp/parallel_execute_file" || die "Error: Fail to create ../tmp/delete_line\n";

if (-e "../tmp/delete_line") {
	unlink "../tmp/delete_line";
}
system "/bin/touch  ../tmp/delete_line" || die "Error: Fail to create ../tmp/delete_line\n";




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
$path_count = 0;
$path_num = 0;
$check1 = 0;
$check2 = 0;
$check3 = 0;
$check4 = 0;
$first=0;
$path_now=0;
$check_all = 1;

open (REPORTFILE, "<$rptfile_name") || die "Error: Can't find the $rptfile_name.\n";
while (<REPORTFILE>){
next if m/^\s*$/;

#change all the keyword such as "header..." "benchmark..." "path..." "a true path ..." "{}" "input vector" to the lower case
#and write these ketword to the file delete_line 
 if($_=~/header.*\{.*a.*true.*path.*set.*\}/i){
  $justify_cmd1=lc($_);
  &write_cmd1;
 }
 elsif($_=~/\{/i){
  $justify_cmd1=lc($_);
  &write_cmd1;
 }
 elsif($_=~/\}/i){
  $justify_cmd1=lc($_);
  &write_cmd1;
 }
 elsif($_=~/A.*True.*Path.*List/i){
  $justify_cmd1=lc($_);
  &write_cmd1;
 }
 elsif($_=~/input.*vector/i){
  $justify_cmd1=lc($_);
  &write_cmd1;
 }
 #the above END!
}
close(REPORTFILE);

open (REPORTFILE, "<../tmp/delete_line") || die "Error: Can't find the delete_line.\n";
@first=<REPORTFILE>;
chomp(@first);
#print "total line =$#first\n";

for($line_delete=0;$line_delete<=$#first;$line_delete++){

#To compare the keyword file (delete_line), delete all space in the file.
$first[$line_delete]=~s/\s//g;#replace all \s to non

}
#print $first[0];
#print $first[1];
#print $first[2];


 
#print "$first[0]";

        #check the header name
        if($first[0] eq "header{atruepathset}"){
	$check1 = 1;
	#print "\nHeader checked!\n";
	}
	else{
	print "\nHeader checked fail *\n";
	exit;
	}
	#check the header name END!
	
	#check the benchmark title name
	if($first[1] eq "benchmark{$case_num}"){
	$check2 = 1;
	#print "\nBenchmark checked!\n";
	}
	else{
	print "\nBenchmark checked fail *\n";
	exit;
	}
	#check the benchmark title name END!
	
	#if($first[2]=~/path\{[\d]\}/s){
	
	#}
	$xx=0;
	while($check_all == 1) {
	if($check2 == 1){
	
	  $path_line=2+(7*$xx);
	    if($first[$path_line]=~/path/i){
	    #print "now is $first[$path_line] \n";
	    
	    #check the "path" name and the path number
	    #if($first[$path_line]=~/([\d].*[^\}])/s)
	    if($first[$path_line]=~/\d+/s)
	    {
	     #$path_now =$1;
	     $path_now =$&;
	   #  print "check path:$path_now ";
	    }
	    } 
	  
	  if($first[$path_line] eq "path{$path_now}"){
	  $check3 = 1;
	  #print "\nPath checked!\n";
	  }
	  else{
	  print "\nPath$path_now fail: Path checked fail *\n";
	  exit;
	  }
	  #check the "path" name and the path number END!
	  
	  
	  
          if($first[$path_line+1] eq "atruepathlist"){
	  $check4 = 1;
	  #print "\nTrue Path List checked!\n";   
	  }
	  else{
	  print "\nPath$path_now fail: True Path List checked fail*\n"; 
	  exit;  
	  }
	  
	  
	  if($first[$path_line+2] eq "{"){
	  $check4 = 1;
	  #print "\n{ checked!\n";   
	  }
	  else{
	  print "\nPath$path_now fail: { checked fail*\n";
	  exit;   
	  }
	  
	  
	  if($first[$path_line+3] eq "}"){
	  $check4 = 1;
	  #print "\n} checked!\n";   
	  }
	  else{
	  print "\nPath$path_now fail: } checked fail*\n"; 
	  exit;  
	  }
	  
	  
	  if($first[$path_line+4] eq "inputvector"){
	  $check4 = 1;
	  #print "\ninputvector checked!\n";   
	  }
	  else{
	  print "\nPath$path_now fail: inputvector checked fail*\n"; 
	  exit;  
	  }
	  
	  
	  if($first[$path_line+5] eq "{"){
	  $check4 = 1;
	  #print "\n{ checked!\n";   
	  }
	  else{
	  print "\nPath$path_now fail: { checked fail*\n"; 
	  exit;  
	  }
	  
	  
	  if($first[$path_line+6] eq "}"){
	  $check4 = 1;
	  #print "\n} checked!\n";   
	  }
	  else{
	  print "\nPath$path_now fail: } checked fail*\n";
	  exit;   
	  }
	  
	  
	  #check there exist the next checked path?
	  if($first[$path_line+7]=~/path/i){
	    if($first[$path_line+7]=~/([\d].*[^\}])/s)
	    {
	     $path_now =$1;
	     #print "$path_now\n";
	    }
	    }
	  #print "path_line=$path_line";
	  if($first[$path_line+7] eq "path{$path_now}"){
	  $check_all = 1;
	  #print "\nNext Path exist!\n";
	  $xx++;
	  }
	  else{
	  $check_all = 0;
	  }
	}
	  
	  
	 
	  
	  
	
	}
	print "\n"; 
	
close(REPORTFILE);

#if the above check fail, the following will stop and the result will fail
open (REPORTFILE, "<$rptfile_name") || die "Error: Can't find the $rptfile_name.\n";
while (<REPORTFILE>){
	chomp($_);
        #s/^\s+|\t/ /g;
	#replace tab to space
	s/\t/ /g;
	#delete the space in the head of line, ^means head
	s/^\s+//g;
	#next if m/^\s*$/;
	#print "$_ \n";
	#$justify_cmd = $_;
	#&write_cmd;
        if(!($_=~/\S+/)) {next;}
	
	if($_=~/path/i){
	if($_=~/([\d].*[^\}])/s)
	{
	$path_num =$1;
	#print $path_num;
	}
	}
	if ($_=~/\(in\)/) {
		#print "\nNotice: Start fetch path.\n";
		$get_path = 1;
		$justify_cmd = "\nreport_timing -input_pins -signifi 0 -slack_lesser_than $slack_less \\\n";
		
	}

	&process_path;
	if ($_=~/\(out\)/) {
		#print "\nNotice: End fetch path.\n";
		$get_path = 0;
		$path_count = $path_count + 1;
		#print "$justify_cmd\n\n";
		&write_cmd;
	}
}

close(REPORTFILE);
}


#################################### 
## Sub
## Process the path
####################################
sub process_path{

if ($get_path == 1){
#print "$_ \n";
	@configitem = split(/\s+/);
	#print "0= $configitem[0] 4= $configitem[4] \n ";
	if ($configitem[1] eq "\(in\)") {
		if ($configitem[4] eq "r") {
			$justify_cmd = $justify_cmd."-rise_from ".$configitem[0]." \\\n";
		} elsif ($configitem[4] eq "f") {
			$justify_cmd = $justify_cmd."-fall_from ".$configitem[0]." \\\n";
		} else {
			#$justify_cmd = $justify_cmd."-from ".$configitem[0]." \\\n";
			#$justify_cmd = $justify_cmd."-XXX ".$configitem[0]." \\\n";
			print "no match (in)";
		}
	} elsif ($configitem[1] eq "\(out\)") {
                if ($configitem[4] eq "r") {
                        $justify_cmd = $justify_cmd."-rise_to ".$configitem[0]." \\\n";
                } elsif ($configitem[4] eq "f") {
                        $justify_cmd = $justify_cmd."-fall_to ".$configitem[0]." \\\n";            
                } else {
                        #$justify_cmd = $justify_cmd."-to ".$configitem[0]." \\\n";
			#$justify_cmd = $justify_cmd."-XXX ".$configitem[0]." \\\n";
			print "no match (in)";
                }
	} else {
                if ($configitem[4] eq "r") {                        
			$justify_cmd = $justify_cmd."-rise_through ".$configitem[0]." \\\n";
                } elsif ($configitem[4] eq "f") {
                        $justify_cmd = $justify_cmd."-fall_through ".$configitem[0]." \\\n";  
                } else {
                        #$justify_cmd = $justify_cmd."-through ".$configitem[0]." \\\n";
			#$justify_cmd = $justify_cmd."-XXX ".$configitem[0]." \\\n";
			print "no match (in)";
                }
	}
	
}

}

#################################### 
## Sub
## Write parallel execute cmd
####################################
sub write_cmd{
#print "Notice: Write the cmd.\n";

open (WRITECMDFILE, ">>../tmp/parallel_execute_file") || die "Error: Can't open parallel_execute_file to write.\n";

#print WRITECMDFILE "redirect -bg -file ".$dir_name."/justify_db/".$path_count.".rpt {".$justify_cmd."}\n\n";
print WRITECMDFILE "echo Path {{ $path_num }}\n\n"."$justify_cmd\n\n";


close (WRITECMDFILE);

}

sub write_cmd1{
#print "Notice: Write the cmd.\n";

open (WRITECMDFILE, ">>../tmp/delete_line") || die "Error: Can't open delete_line to write.\n";

#print WRITECMDFILE "redirect -bg -file ".$dir_name."/justify_db/".$path_count.".rpt {".$justify_cmd."}\n\n";
print WRITECMDFILE "$justify_cmd1";


close (WRITECMDFILE);

}
