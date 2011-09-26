<?php
/* This file is part of Springfiles (GPL v2 or later), see LICENSE.txt */

/**
 * @author abma <spam@abma.de>
 * @file
 * used for the cron job
 */

function _dlc_get_lastrun(){
	return intval(db_result(db_query("SELECT value from {dlc_config} WHERE name='dlc_last_run'")));
}

function _dlc_set_lastrun($lastid){
	$res=db_query("UPDATE {dlc_config} SET value=%d WHERE name='dlc_last_run'",$lastid);
	if (db_affected_rows()<=0)
		db_query("INSERT INTO {dlc_config} (name, value) VALUES ('dlc_last_run', %d)",$lastid);
}



/**
 *  reads the table download_count and fills dlc_day + dlc_file with the data to generate some statistics
 */

function dlc_cron_run(){
	//try to disable execution time-limit
	if (function_exists('set_time_limit')) {
		@set_time_limit(0);
	}
	$lastrun=_dlc_get_lastrun();
	$result=db_query("SELECT dcid,fid,nid,vid,timestamp FROM {download_count} WHERE timestamp > %d ORDER by timestamp", $lastrun);
	$updates=array();
	//for each entry in the log, increment the stat-tables
	while ($row = db_fetch_array($result)) {
		if ($lastrun<$row['timestamp']){
			$lastrun=$row['timestamp'];
			//set start point for next run, set befor any updates to avoid duplicates, this is slow, but without duplicate entries could happen
			_dlc_set_lastrun($lastrun);
		}

		$day=$row['timestamp'] - ($row['timestamp'] % 86400); //round to full day
		$week= date('o', $row['timestamp']).date('W', $row['timestamp']); // set week to year.weeknumber
		$month= date('Y', $row['timestamp']).date('n', $row['timestamp']); //set month to year.month
		$year= date('Y', $row['timestamp']); //set year from timestamp
		//insert/update into daily table
		$res=db_query("UPDATE {dlc_day} SET count=count+1 WHERE day=%d",$day);
		if (db_affected_rows()<=0)
			$res=db_query("INSERT  INTO {dlc_day} (count, day) VALUES (%d, %d)",array(1,$day));

		//insert/update into file table
		$res=db_query("UPDATE {dlc_file} SET count=count+1 WHERE fid=%s",$row['fid']);
		if (db_affected_rows()<=0)
			$res=db_query("INSERT  INTO {dlc_file} (count, fid, nid, vid) VALUES (%d, %d, %d, %d)",array(1,$row['fid'],$row['nid'],$row['vid']));
		
		//insert/update into file day table
		$res=db_query("UPDATE {dlc_file_day} SET count=count+1 WHERE fid=%s AND day = %d",array($row['fid'], $day));
		if (db_affected_rows()<=0)
			$res=db_query("INSERT  INTO {dlc_file_day} (count, fid, day) VALUES (%d, %d, %d)",array(1,$row['fid'], $day));
			
		//insert/update into file week table
		$res=db_query("UPDATE {dlc_file_week} SET count=count+1 WHERE fid=%s AND week = %d",array($row['fid'], $week));
		if (db_affected_rows()<=0)
			$res=db_query("INSERT  INTO {dlc_file_week} (count, fid, week) VALUES (%d, %d, %d)",array(1,$row['fid'], $week));
			
		//insert/update into file month table
		$res=db_query("UPDATE {dlc_file_month} SET count=count+1 WHERE fid=%s AND month = %d",array($row['fid'], $month));
		if (db_affected_rows()<=0)
			$res=db_query("INSERT  INTO {dlc_file_month} (count, fid, month) VALUES (%d, %d, %d)",array(1,$row['fid'], $month));
		
		//insert/update into file year table
		$res=db_query("UPDATE {dlc_file_year} SET count=count+1 WHERE fid=%s AND year = %d",array($row['fid'], $year));
		if (db_affected_rows()<=0)
			$res=db_query("INSERT  INTO {dlc_file_year} (count, fid, year) VALUES (%d, %d, %d)",array(1,$row['fid'], $year));
	}

}

