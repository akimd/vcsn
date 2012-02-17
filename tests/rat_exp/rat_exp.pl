#! /usr/bin/perl
my $blue = `tput setaf 4`;
my $cyan = `tput setaf 6`;
my $white   = `tput setaf 7`;
my $green = `tput setaf 2`;
my $PASS  = "${green}PASS$white";
my $red  = `tput setaf 1`;
my $FAIL = "${red}XFAIL$white";
my $fail = 0;
sub quote ($) {
    my ($in) = @_;
    $in =~ s/(["$\\`])/\\$1/g;
    $in =~ s/^[ \t]*//g;
    $in =~ s/[ \t]*$//g;
    $in;
}

sub check_rat_exp {
    my ($verbose, @file) = @_;

    foreach(@file) {
        open(IN, $_);
        foreach(<IN>) {
            $_ =~ m/\|([^|]*)\|([^|]*)\|/;
            my $l = quote $1;
            my $r = quote $2;
            my $L = `echo  '$l' | ./main`;
            chomp($L);
            my $R = `echo  '$r' | ./main`;
            chomp($R);

            if($L eq $R) {
                print "$PASS: $l == $r\n";
            }
            else {
                print "$FAIL: $l == $L != $r == $R\n";
                if($verbose) {
                    print "$l:\n";
                    `echo  '$l' | ./main`;
                    print "$r:\n";
                    `echo  '$r' | ./main`;
                }
                ++$fail;
            }
        }
    }
}
my $verbose = 0;
check_rat_exp($verbose, @ARGV);

exit !!$fail;
