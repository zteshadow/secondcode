
#!/usr/local/bin/perl
use File::Find;
use File::Path;
use File::Copy;
use Cwd;

$file_name_cnt;
$file_processed;

@old_fname;
@new_fname;


if (@ARGV)
{
    get_name_list($ARGV[0]);
    
    process_files($ARGV[0]);
    
    print "\n-----------------------------------\n";
    print "$file_name_cnt files name need change and $file_processed files processed\n";
}
else
{
    die "input the directory\n";
}

sub check_name
{
    my $name = $_;                  	#file name
	my $old_name = $name;

    if ($name =~ /\.(c|cpp|m|h)$/)      #source file
	{
		#'cm' -> 'bd'
		if ($name =~ /^cm[^a-z^A-Z^\.]/)
		{
			$file_name_cnt++;

			$name =~ s/cm/bd/;			#keyword
			print $old_name." -> ".$name."\n";
			
			push(@old_fname, $old_name);
			push(@new_fname, $name);
			
			rename $old_name, $name;
		}
	}
}

#input 'dir'
sub get_name_list
{
    my $dir = $_[0];
    my $cnt;
    
    print "get_name_list: ". $dir."\n";
    
    find(\&check_name, $dir);
}

sub do_substitute
{
    my $src_name = $_;                  						#file name
	my $dst_name = $src_name.".samuel";

    if ($src_name =~ /\.(c|cpp|m|h|vcproj|pbxproj|pbxuser)$/)	#source file and project file
	{
		$file_processed++;
		
		print "src file: ".$src_name."\n";
		#print "dst file: ".$dst_name."\n";

		unlink $dst_name;

		open my $src, "< $src_name" || die "can't open $src_name $!\n";	
		open my $dst, "> $dst_name" || die "can't open $dst_name $!\n";

		while (<$src>)
		{
			my $line = $_;
			my $old, $new;

			my $cnt = @old_fname;
			#print "\nTotal: ".$cnt."\n";
			my $index = 0;
			while ($index < $cnt)
			{
				$old = $old_fname[$index];
				$new = $new_fname[$index];

				$old =~ s/\./\\\./g;			#change '.' to '\.'
				#print $old." -> ".$new."\n";
				$line =~ s/$old/$new/;

				#process include macro
				if ($old =~ /\.(h)$/)
				{
					$old = uc($old);
					$old =~ s/\./_/g;
					$old = "_".$old;

					$new = uc($new);
					$new =~ s/\./_/g;
					$new = "_".$new;

					$line =~ s/$old/$new/;
				}

				$index = $index + 1;
			}		

			#change symbol
			$line =~ s/\bcm_/bd_/g;

			#change type and macro
			$line =~ s/\bCM_/BD_/g;

			print $dst $line;	
			#print $line;
		}

		close $src;
		close $dst;

		unlink $src_name;
		rename $dst_name, $src_name;
	}
}

sub process_files
{
    my $dir = $_[0];
    
    #print "\nchange_file_name: ". $dir."\n";
    
    find(\&do_substitute, $dir);
}

