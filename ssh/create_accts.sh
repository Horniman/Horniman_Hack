ls *.pub | while read acct
do
  id=$(echo $acct | sed 's/.pub//')
  useradd -d /home/$id $id
  mkdir -p /home/$id/.ssh
  chown -R $id /home/$id
  chmod 600 /home/$id/.ssh/authorized_keys
  cat $acct > /home/$id/.ssh/authorized_keys
done
