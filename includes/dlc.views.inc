<?php
/**
 * Implementation of hook_views_data().
 */
function dlc_views_data() {

  //base table info
  $data['dlc_file']['table']['group']  = t('DLC');
  $data['dlc_file']['table']['base'] = array(
    'field' => 'fid',
    'title' => t('File download'),
    'help' => t("Download history for files handled by the core upload and contributed filefield modules."),
  );

  //joins
  $data['dlc_file']['table']['join'] = array(
    'files' => array(
      'left_field' => 'fid',
      'field' => 'fid',
    ),
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
    'node_revisions' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
  );

  //fields
  $data['dlc_file']['fid'] = array(
    'title' => t('Download count id'),
    'help' => t('The unique id of the download.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['dlc_file']['count'] = array(
    'title' => t('count'),
    'help' => t('Number of downloads.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  //relationships
  $data['dlc_file']['fid'] = array(
    'title' => t('File'),
    'help' => t('Relate the download to the file.'),
    'relationship' => array(
      'base' => 'files',
      'base field' => 'fid',
      'handler' => 'views_handler_relationship',
      'label' => t('File'),
    ),
  );

  return $data;
}