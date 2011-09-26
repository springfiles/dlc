<?php
/* This file is part of Springfiles (GPL v2 or later), see LICENSE.txt */

/**
 * @author abma <spam@abma.de>
 * @file
 * used for all output of the dlc-module
 */


/**
 *	creates a progressbar
 *	@param $width is % it is filled
 *	@param $maxnumber is the max possible amout
 *	@param $show_text is the text displayed
 *	@param $table_width is the width of the table
 */
function create_bar($width, $maxnumber, $show_text="",$table_width=670){
	$bar = "";
	if ($maxnumber)
		$percent = ($width / $maxnumber);
	else
		$percent=0;
	//width in pixel (0-100)
	$width = round($table_width * $percent);
	//brightest color:#114 236  35
	//darkest color:  # 20  40  00
	$color = sprintf('#%02X%02X%02X', 20+round($percent *94),40 + round($percent*196),round($percent*35));

	$bar .= '
	<table style="width: '.$table_width.'px; height:8px; border: 1px solid #333333; " cellpadding="0" cellspacing="0">
	<tr>
		<td style="background-color:#222222; height: 8px;">
			<div style="width:'.$width.'px; background-color:'.$color.'; color: #000; line-height: 0.8em; font-size: 9px; text-align: right; height: 9px; margin: 0 0 0 0; padding: 2px 0 0 0; ">'.$show_text.'</div>
		</td>
	</tr>
	</table>
	';
	
	return $bar;
}

function _dlc_create_head($title){
	return "<h2>".$title."</h2>";
}

/**
 *	displays an 2 week stat
 *	@param $date the last date
 *	@param $days how many days are shown
 */
function _dlc_display_daily($date, $days=100){
	//normalize date
	$output=_dlc_create_head("Downloads last $days days");
	$date = $date - ($date % 86400);
	$end = $date - $days * 86400;
	$result = db_query("select * from {dlc_day} where day<=%d and day>=%d ORDER BY day DESC", $date, $end );
	$arr=array();
	$max=0;
	while ($row = db_fetch_array($result)) {
			if ($max<$row['count'])
					$max=$row['count'];
			$arr[]=$row;
	}
	foreach($arr as $row){
		$output.= "<table><tr>";
		$output.= "<td width=75>".strftime("%Y.%m.%d",$row['day'])."</td>";
		$output.= "<td>".create_bar($row['count'], $max, $row['count'] )."</td>";
		$output.= "</tr></table>";
	}
	return $output;
}

/**
 *	display monthly statistics
 *	@param $date is the startdate
 *	@param $months is count of months to display
 */
function _dlc_display_monthly($date, $months=48){
	$output=_dlc_create_head("Downloads last $months months");
	$montsec=2678400; //seconds a month has
	$enddate=$date - ($montsec*$months);
	$result=db_query("SELECT day, FROM_UNIXTIME(day,'%Y/%m') as month, sum(count) as sumcount FROM {dlc_day} WHERE day<=%d and day>=$enddate GROUP by month ORDER by month DESC",$date, $enddate);
	$arr=array();
	$max=0;
	while ($row = db_fetch_array($result)) {
			if ($max<$row['sumcount'])
					$max=$row['sumcount'];
			$arr[]=$row;
	}
	foreach($arr as $row){
		$output.= "<table><tr>";
		$output.= "<td width=75>".$row['month']."</td>";
		$output.= "<td>".create_bar($row['sumcount'], $max, $row['sumcount'] )."</td>";
		$output.= "</tr></table>";
	}
	return $output;
}
function _dlc_display_yearly($date, $years=10){
	$output=_dlc_create_head("Downloads last $years years");
	$yearsec=32140800; //seconds a year has
	$enddate=$date - ($yearsec*$years);
	$result=db_query("SELECT day, FROM_UNIXTIME(day,'%Y') as year, sum(count) as sumcount FROM {dlc_day} WHERE day<=%d and day>=$enddate GROUP by year ORDER by year DESC",$date, $enddate);
	$arr=array();
	$max=0;
	while ($row = db_fetch_array($result)) {
			if ($max<$row['sumcount'])
					$max=$row['sumcount'];
			$arr[]=$row;
	}
	foreach($arr as $row){
		$output.= "<table><tr>";
		$output.= "<td width=75>".$row['year']."</td>";
		if ($row['year'] == 2009) $row['sumcount'] *= 5;
		$output.= "<td>".create_bar($row['sumcount'], $max, $row['sumcount'] )."</td>";
		$output.= "</tr></table>";
	}
	return $output;
}

/**
 *	display monthly statistics
 *	@param $date is the startdate
 *	@param $weeks is count of months to display
 */
function _dlc_display_weekly($date, $weeks=54){
	
	$output=_dlc_create_head("Downloads last $weeks weekls");
	$weeksec=604800; //seconds a week has
	$enddate=$date - ($weeksec*$weeks);
	$result=db_query("SELECT day, FROM_UNIXTIME(day,'%Y/%v') as week, sum(count) as sumcount FROM {dlc_day} WHERE day<=%d and day>=%d GROUP by week ORDER by week DESC",$date, $enddate);
	$arr=array();
	$max=0;
	while ($row = db_fetch_array($result)) {
			if ($max<$row['sumcount'])
					$max=$row['sumcount'];
			$arr[]=$row;
	}
	unset($arr[0]);
	foreach($arr as $key => $row){
		$output.= "<table><tr>";
		$output.= "<td width=75>".$row['week']."</td>";
		$output.= "<td>".create_bar($row['sumcount'], $max, $row['sumcount'] )."</td>";
		$output.= "</tr></table>";
	}
	return $output;
}

/**
 *	display daily stats
 */
function dlc_display(){
	$date=time();
	$output.= _dlc_display_daily($date);
	$output.= _dlc_display_weekly($date);
	$output.= _dlc_display_monthly($date);
	$output.= _dlc_display_yearly($date);
	return $output;
}

/**
 *	not used
 */

function _dlc_last_downloaded($amount = 6){
	
	$node = node_load(arg(1));
	
	if ($node->path == 'games' || $node->path == 'demos' || $node->path == 'spellen' || $node->path == 'linux' || $node->path == 'mac')
	{
		$result=db_query("SELECT d.nid, d.timestamp FROM {download_count} d
			 INNER JOIN {content_type_file} f ON f.nid = d.nid
			 INNER JOIN {content_field_category} fc on f.nid = fc.nid
			 INNER JOIN {content_type_subcategory} s ON fc.field_category_nid = s.nid
			 INNER JOIN {content_type_category} c ON s.field_category_ref_nid = c.nid
			 WHERE c.field_games_nid = %d AND d.referrer LIKE 'http://".$_SERVER['SERVER_NAME']."%'
			 ORDER BY d.timestamp DESC LIMIT 0,6",array($node->nid));
	}
	elseif ($node->path == 'frontpage' || $node->path == 'hot-games')
	{
		$result=db_query("SELECT d.nid, d.timestamp FROM {download_count} d
				 INNER JOIN {content_type_file} f ON f.nid = d.nid
				 WHERE d.referrer LIKE 'http://".$_SERVER['SERVER_NAME']."%'
				 ORDER BY d.timestamp DESC LIMIT 0,6");    
	}
	elseif ($node->type == 'subcategory')
	{
		$result=db_query("SELECT d.nid, d.timestamp FROM {download_count} d
			 INNER JOIN {content_type_file} f ON f.nid = d.nid
			 INNER JOIN {content_field_category} fc on f.nid = fc.nid
			 WHERE fc.field_category_nid = %d AND d.referrer LIKE 'http://".$_SERVER['SERVER_NAME']."%'
			 ORDER BY d.timestamp DESC LIMIT 0,6",array($node->nid));
	}
	else {
	$result=db_query("SELECT d.nid, d.timestamp FROM {download_count} d
			 INNER JOIN {content_type_file} f ON f.nid = d.nid
			 INNER JOIN {content_field_category} fc on f.nid = fc.nid
			 INNER JOIN {content_type_category} c ON fc.field_category_nid = c.nid
			 WHERE d.referrer LIKE 'http://".$_SERVER['SERVER_NAME']."%' AND (c.field_games_nid = %d OR fc.field_category_nid = %d)
			 ORDER BY d.timestamp DESC LIMIT 0,6",array($node->nid, $node->nid));
	}
	$arr=array();
	$max=0;
	while ($row = db_fetch_array($result)) {
		$item = node_load($row['nid']);
		$item->title = _nicetrim($item->title);
		$output .= '<span class="last_download_title"><a href = "/'.$item->path.'">'.$item->title.'</a></span>'._dlc_ago($row['timestamp']).'<br/>';
	}
	return $output;
}

function _dlc_ago($timestamp){
  return format_interval(time() - $timestamp, 1). ' '. t('ago');
}

function _nicetrim ($s) {
// limit the length of the given string to $MAX_LENGTH char
// If it is more, it keeps the first $MAX_LENGTH-3 characters
// and adds "..."
// It counts HTML char such as &aacute; as 1 char.
//

  $MAX_LENGTH = 28;
  $str_to_count = html_entity_decode($s);
  if (strlen($str_to_count) <= $MAX_LENGTH) {
    return $s;
  }

  $s2 = substr($str_to_count, 0, $MAX_LENGTH - 3);
  $s2 .= "...";
  return htmlentities($s2);
} 

