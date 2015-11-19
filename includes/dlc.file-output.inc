<?php
/* This file is part of Springfiles (GPL v2 or later), see LICENSE.txt */

/**
 * @author jj <job@jobjol.com> & abma <spam@abma.de>
 * @file
 * used for the cron job
 */

/**
 *  generates some statistics blocks/pages for the file pages
 */

function dlc_nodeapi(&$node, $op, $teaser) {
  $result = array();
  switch ($op) {
    case 'view' :
      
      //get all filefield files
      if (module_exists('filefield'))
      {
        foreach (filefield_get_field_list($node->type) as $fieldname => $filefield_data)
        {
          if (count($node->$fieldname))
          {
          foreach($node->$fieldname as &$filefield)
            {
              //attach mirror data to the filefield array
              $downloads = db_fetch_object(db_query("SELECT count FROM {dlc_file} WHERE fid = %d", $filefield['fid']));
              $filefield['downloads'] = $downloads->count;
            }
          }
        }
      }
      break;
  }
}


/**
* generated daily stats output.
* arguments: @amount = the number of days to show
*            @fid = the file id to base the stats on
*/
function dlc_file_stats_day($amount = 16, $fid) {
   $date=time();
   $date = $date - ($date % 86400);
   $end = $date - $amount * 86400;
   
   $result = db_query("select * from {dlc_file_day} where day<=%d AND day>=%d AND fid = %d ORDER BY day DESC", $date, $end, $fid );
	$arr=array();
	$max=0;
	while ($row = db_fetch_array($result)) {
			if ($max<$row['count'])
					$max=$row['count'];
			$arr[]=$row;
	}
	foreach($arr as $row){
		$output.= "<table><tr>";
		$output.= "<td>".strftime("%Y.%m.%d",$row['day'])."</td>";
		$output.= "<td>".create_bar($row['count'], $max, $row['count'], 128 )."</td>";
		$output.= "</tr></table>";
	}
	return $output;
}

/**
* generated weekly stats output.
* arguments: @amount = the number of days to show
*            @fid = the file id to base the stats on
*/
function dlc_file_stats_week($amount = 10, $fid) {
    $result = db_query("select * from {dlc_file_week} WHERE fid = %d ORDER BY week DESC LIMIT 0, %d ", array($fid, $amount));
    $arr=array();
    $max=0;
    while ($row = db_fetch_array($result)) {
                    if ($max<$row['count'])
                                    $max=$row['count'];
                    $arr[]=$row;
    }
    foreach($arr as $row){
            $output.= "<table><tr>";
            $output.= "<td>".substr($row['week'],4)."</td>";
            $output.= "<td>".create_bar($row['count'], $max, $row['count'], 170 )."</td>";
            $output.= "</tr></table>";
    }
    return $output;   
}

/**
* generated monthly stats output.
* arguments: @amount = the number of days to show
*            @fid = the file id to base the stats on
*/
function dlc_file_stats_month($amount = 24,$fid) {
    $result = db_query("select * from {dlc_file_month} WHERE fid = %d", array($fid, $amount));
    $arr=array();
    $max=0;
    while ($row = db_fetch_array($result)) {
                    if ($max<$row['count'])
                                    $max=$row['count'];
                    $row['y'] = substr($row['month'],0,4);
		    $row['m'] = substr($row['month'],4,2);
		    $arr[]=$row;
    }
    $arr = array_reverse(multisort($arr, array('y','m')));
    foreach($arr as $row){
            $output.= "<table><tr>";
            $output.= '<td width="55">'.substr($row['month'],0,4).'-'.substr($row['month'],4,2)."</td>";
            $output.= "<td>".create_bar($row['count'], $max, $row['count'], 140 )."</td>";
            $output.= "</tr></table>";
	    $i++;
	    if ($i == $amount) break;
    }
    return $output;      
}

function multisort($array, $sort_by) {
    foreach ($array as $key => $value) {
        $evalstring = '';
        foreach ($sort_by as $sort_field) {
            $tmp[$sort_field][$key] = $value[$sort_field];
            $evalstring .= '$tmp[\'' . $sort_field . '\'], ';
        }
    }
    $evalstring .= '$array';
    $evalstring = 'array_multisort(' . $evalstring . ');';
    eval($evalstring);

    return $array;
}


/**
* generated yearly stats output.
* arguments: @amount = the number of days to show
*            @fid = the file id to base the stats on
*/
function dlc_file_stats_year($amount = 10,$fid) {
    $result = db_query("select * from {dlc_file_year} WHERE fid = %d ORDER BY year DESC LIMIT 0, %d ", array($fid, $amount));
    $arr=array();
    $max=0;
    while ($row = db_fetch_array($result)) {
                    if ($max<$row['count'])
                                    $max=$row['count'];
                    $arr[]=$row;
    }
    foreach($arr as $row){
            $output.= "<table><tr>";
            $output.= "<td>".$row['year']."</td>";
            if ($row['year'] == 2009) $row['count'] *= 3;
	    
	    $output.= "<td>".create_bar($row['count'], $max, $row['count'], 158 )."</td>";
            $output.= "</tr></table>";
    }
    return $output;   
}

