. ./regress.conf

#TODO move this to ../replica_check/replica_check-common.sh

NCOPY1=3  # NCOPY1 >= 3
NCOPY2=2  # NCOPY2 < NCOPY1
NCOPY_TIMEOUT=10  # sec.
hostgroupfile=$localtop/.hostgroup.$$

MAINCTRL=
REPREMOVE=
REDUCED_LOG=
GRACE_SPACE_RATIO=
GRACE_TIME=
HOST_DOWN_THRESH=
SLEEP_TIME=
MINIMUM_INTERVAL=

replica_check_setup_test() {
  tmpf=$gftmp/foo
  replica_check_check_supported_env
  trap 'replica_check_clean_test; exit $exit_trap' $trap_sigs
  replica_check_del_testdir
  gfmkdir $gftmp || exit $exit_fail
  replica_check_backup_hostgroup
  replica_check_backup_repcheck_conf
  replica_check_setup_test_ncopy
  replica_check_setup_test_repattr
  replica_check_setup_test_conf
}

replica_check_del_testdir() {
  gfrm -rf $gftmp
}

replica_check_clean_test() {
  replica_check_restore_hostgroup
  replica_check_restore_repcheck_conf
  replica_check_del_testdir
}

replica_check_check_supported_env() {
  if $regress/bin/am_I_gfarmadm; then
    # can use "gfhostgroup -s/-r" and gfrepcheck
    :
  else
    exit $exit_unsupported
  fi
  hosts=`gfsched -w`
  if [ $? -ne 0 -o "X${hosts}" = "X" ]; then
    exit $exit_fail
  fi
  nhosts=`echo ${hosts} | wc -w`
  if [ $nhosts -lt $NCOPY1 ]; then
    echo 2>&1 "nhosts = $nhosts < $NCOPY1"
    exit $exit_unsupported
  fi
}

replica_check_backup_hostgroup() {
  if (gfhostgroup | sed 's/:/ /' > ${hostgroupfile}); then
    :
  else
    exit $exit_fail
  fi
}

replica_check_restore_hostgroup() {
  if [ -r $hostgroupfile ]; then
    while read h g; do
      if [ "X${g}" != "X" ]; then
        gfhostgroup -s $h $g
      else
        gfhostgroup -r $h
      fi
    done < $hostgroupfile
    rm -f $hostgroupfile
  fi
}

replica_check_backup_repcheck_conf() {
  MAINCTRL=`gfrepcheck status | awk '{print $1}'`
  REPREMOVE=`gfrepcheck remove status`
  REDUCED_LOG=`gfrepcheck reduced_log status`
  GRACE_SPACE_RATIO=`gfrepcheck remove_grace_used_space_ratio status`
  GRACE_TIME=`gfrepcheck remove_grace_time status`
  HOST_DOWN_THRESH=`gfrepcheck host_down_thresh status`
  SLEEP_TIME=`gfrepcheck sleep_time status`
  MINIMUM_INTERVAL=`gfrepcheck minimum_interval status`

  if [ -z "$MAINCTRL" -o -z "$REPREMOVE" -o -z "$REDUCED_LOG" -o \
       -z "$GRACE_SPACE_RATIO" -o -z "$GRACE_TIME" -o \
       -z "$HOST_DOWN_THRESH" -o -z "$SLEEP_TIME" -o \
       -z "$MINIMUM_INTERVAL" ]; then
      echo "gfrepcheck cannot get current status."
      exit $exit_fail
  fi
}

replica_check_restore_common() {
  CONF=$1
  VAL=$2
  if [ -n "$VAL" ]; then
    if [ -z "$CONF" ]; then
      gfrepcheck "$VAL"
    else
      gfrepcheck "$CONF" "$VAL"
    fi
  fi
}

replica_check_restore_repcheck_conf() {
  replica_check_restore_common "" "$MAINCTRL"
  MAINCTRL=
  replica_check_restore_common "remove" "$REPREMOVE"
  REPREMOVE=
  replica_check_restore_common "reduced_log" "$REDUCED_LOG"
  REDUCED_LOG=
  replica_check_restore_common "remove_grace_used_space_ratio" \
    "$GRACE_SPACE_RATIO"
  GRACE_SPACE_RATIO=
  replica_check_restore_common "remove_grace_time" "$GRACE_TIME"
  GRACE_TIME=
  replica_check_restore_common "host_down_thresh" "$HOST_DOWN_THRESH"
  HOST_DOWN_THRESH=
  replica_check_restore_common "sleep_time" "$SLEEP_TIME"
  SLEEP_TIME=
  replica_check_restore_common "minimum_interval" "$MINIMUM_INTERVAL"
  MINIMUM_INTERVAL=
}

replica_check_setup_test_ncopy() {
  if set_ncopy 1 $gftmp &&  ### avoid looking parent gfarm.ncopy
    gfreg $data/1byte $tmpf; then
    :
  else
    replica_check_clean_test
    exit $exit_fail
  fi
}

replica_check_setup_test_repattr() {
  for h in $hosts; do
    if gfhostgroup -s $h test0; then
      :
    else
      replica_check_clean_test
      exit $exit_fail
    fi
  done

  if gfncopy -S test0:1 $gftmp && # avoid looking parent gfarm.replicainfo
    gfreg $data/1byte $tmpf; then
    :
  else
    replica_check_clean_test
    exit $exit_fail
  fi
}

replica_check_setup_test_conf() {
  if gfrepcheck enable && \
     gfrepcheck sleep_time 0 && \
     gfrepcheck minimum_interval 1 && \
     gfrepcheck remove enable && \
     gfrepcheck remove_grace_used_space_ratio 0 && \
     gfrepcheck remove_grace_time 0; then
    :
  else
    replica_check_clean_test
    exit $exit_fail
  fi
}

replica_check_wait_for_expected_status() {
  expected="$1"
  count=0
  while true; do
    status=`gfrepcheck status`
    if [ $? -ne 0 ]; then
      replica_check_clean_test
      exit $exit_fail
    fi
    if [ "$status" = "$expected" ]; then
      break
    fi
    count=`expr ${count} + 1`
    echo "sleep [${count}]: waiting for '${expected}' of 'gfrepcheck status'"
    sleep 1
  done
}

replica_check_wait_for_finished() {
  if gfrepcheck stop; then
    :
  else
    replica_check_clean_test
    exit $exit_fail
  fi
  replica_check_wait_for_expected_status "disable / stopped"
  if gfrepcheck start; then
    :
  else
    replica_check_clean_test
    exit $exit_fail
  fi
  minimum_interval=`gfrepcheck minimum_interval status`
  if [ $? -ne 0 ]; then
    replica_check_clean_test
    exit $exit_fail
  fi
  sleep ${minimum_interval}
  replica_check_wait_for_expected_status "enable / stopped"
}

replica_check_wait_for_rep() {
  num=$1
  file=$2
  expect_timeout=$3
  diag=$4
  WAIT_TIME=0

  replica_check_wait_for_finished
  while
    if [ `gfncopy -c $file` -eq $num ]; then
      if [ $expect_timeout = 'true' ]; then
        echo -n "replicas: "
        gfwhere $file
        echo "unexpected: Timeout must occur."
        replica_check_clean_test
        exit $exit_fail
      fi
      exit_code=$exit_pass
      false # exit from this loop
    else
      true
    fi
  do
    WAIT_TIME=`expr $WAIT_TIME + 1`
    if [ $WAIT_TIME -gt $NCOPY_TIMEOUT ]; then
      echo "replication timeout: ${diag}"
      if [ $expect_timeout = 'true' ]; then
        return
      fi
      replica_check_clean_test
      exit $exit_fail
    fi
    sleep 1
  done
}

set_ncopy() {
  if gfncopy -s $1 $2; then
    :
   else
    echo gfncopy -s failed
    replica_check_clean_test
    exit $exit_fail
  fi
}

set_repattr() {
  gfncopy -S test0:$1 $2
  if [ $? -ne 0 ]; then
    echo gfncopy -S failed
    replica_check_clean_test
    exit $exit_fail
  fi
}

hardlink() {
  gfln $1 $2
  if [ $? -ne 0 ]; then
    replica_check_clean_test
    exit $exit_fail
  fi
}

gfprep_n() {
  NCOPY=$1
  FILE=$2
  if gfprep $GFPREP_OPT -N $NCOPY gfarm:${FILE}; then
    :
  else
    echo failed: gfprep $GFPREP_OPT -N $NCOPY gfarm:${FILE}
    replica_check_clean_test
    exit $exit_fail
  fi
}

set_grace_used_space_ratio() {
  RATIO=$1
  if gfstatus -Mm "replica_check_remove_grace_used_space_ratio ${RATIO}"; then
    :
  else
    echo failed: "replica_check_remove_grace_used_space_ratio ${RATIO}"
    replica_check_clean_test
    exit $exit_fail
  fi
}

set_grace_time() {
  SEC=$1
  if gfstatus -Mm "replica_check_remove_grace_time ${SEC}"; then
    :
  else
    echo failed: "replica_check_remove_grace_time ${SEC}"
    replica_check_clean_test
    exit $exit_fail
  fi
}

replica_check_remove_switch()
{
  FLAG=$1
  if gfrepcheck remove $FLAG; then
    :
  else
    echo failed: "gfrepcheck remove $FLAG"
    replica_check_clean_test
    exit $exit_fail
  fi
}
