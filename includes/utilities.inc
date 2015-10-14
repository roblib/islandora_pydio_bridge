<?php

/**
 * @file
 * Utility functions
 */

/**
 * Parse the DDI looking for the a:AccessCondition elements Content value.
 * 
 * @param string $ddi
 * @return string
 *   Returns the DDI accesscondition/content value or NULL
 */
function islandora_pydio_bridge_get_ddi_status($ddi) {
  $xml = new DOMDocument();
  $test = $xml->loadXML($ddi);
  $query = "//a:AccessConditions/r:Content";
  $xpath = new DOMXPath($xml);
  $xpath->registerNamespace('r', 'ddi:reusable:3_2');
  $xpath->registerNamespace('a', 'ddi:archive:3_2');
  $results = $xpath->query($query, $xml);
  if ($results->length > 0) {
    return $results->item(0)->nodeValue;
  }
  return NULL;
}

/**
 * Parse the DDI looking for the r:IdentifierContent value.
 * 
 * @param string $ddi
 * @return string
 *   Returns the IdentifierContent value or NULL
 */
function islandora_pydio_bridge_get_ddi_identifier($ddi) {
  $xml = new DOMDocument();
  $test = $xml->loadXML($ddi);
  $query = "//r:Citation/r:InternationalIdentifier[r:ManagingAgency/text() = 'Datacite']/r:IdentifierContent";
  $xpath = new DOMXPath($xml);
  $xpath->registerNamespace('r', 'ddi:reusable:3_2');
  $results = $xpath->query($query, $xml);
  if ($results->length > 0) {
    return $results->item(0)->nodeValue;
  }
  return NULL;
}
