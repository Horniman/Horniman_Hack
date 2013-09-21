ls *.pub | while read acct
do
  useradd $(echo $acct | sed 's/.pub//')
    cat $acct > /home/$(echo $acct | sed 's/.pub//')/.ssh/authorized_keys
    done
