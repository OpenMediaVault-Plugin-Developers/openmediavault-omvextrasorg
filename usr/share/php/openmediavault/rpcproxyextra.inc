<?php
/**
 *
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2015 Volker Theile
 * @copyright Copyright (c) 2015 OpenMediaVault Plugin Developers
 *
 * OpenMediaVault is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * OpenMediaVault is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenMediaVault. If not, see <http://www.gnu.org/licenses/>.
 */
require_once("openmediavault/rpcproxy.inc");


/**
 * A specialized RPC proxy that extends the original upload class
 * to handle multiple file uploads and optionally enables secure
 * deletion of the temporary files.
 */
class OMVExtrasUploadRpcProxy extends OMVUploadRpcProxy {
    protected $secureDeletion = FALSE;
    protected $maxFileSize = FALSE;

    protected function codeToMessage($code) {
        switch ($code) {
            case UPLOAD_ERR_INI_SIZE:
                $message = "The uploaded file exceeds the upload_max_filesize directive in php.ini";
                break;
            case UPLOAD_ERR_FORM_SIZE:
                $message = "The uploaded file exceeds the maximum size allowed";
                if($this->maxFileSize !== FALSE)
                    $message = sprintf($message." (%d MB)",
                                        $this->maxFileSize/1048576);
                break;
            case UPLOAD_ERR_PARTIAL:
                $message = "The uploaded file was only partially uploaded";
                break;
            case UPLOAD_ERR_NO_FILE:
                $message = "No file was uploaded";
                break;
            case UPLOAD_ERR_NO_TMP_DIR:
                $message = "The system temporary folder is missing";
                break;
            case UPLOAD_ERR_CANT_WRITE:
                $message = "Failed to write file to disk";
                break;
            case UPLOAD_ERR_EXTENSION:
                $message = "File upload stopped by extension";
                break;

            default:
                $message = "Unknown upload error";
                break;
        }
        return $message;
    }

    protected function getParams() {
        // Decode the params (sub-)array
        if(parent::getParams() === FALSE)
            return FALSE;
        // Decode the options array
        $this->getOptions();
        // Set shred on cleanup option if it was specified
        if(array_key_exists("options", $this->params) && !is_null(
            $this->params['options']) && array_key_exists("secureDeletion",
            $this->params['options'])) {
                $this->secureDeletion =
                    boolval($this->params['options']['secureDeletion']);
        }
        // If MAX_FILE_SIZE is set, store the value for use in
        // error messages (see codeToMessage function), and then
        // remove the field from the params array before validation.
        if(array_key_exists("MAX_FILE_SIZE", $this->params)) {
            $this->maxFileSize = $this->params['MAX_FILE_SIZE'];
            $this->debug(sprintf("Max upload file size set to %d MB",
                                    $this->maxFileSize/1048576));
            unset($this->params['MAX_FILE_SIZE']);
        }
        // Move non-file fields into params array before validation
        $this->mergeFields();
        return TRUE;
    }

    /**
     * Decode the options array
     */
    protected function getOptions() {
        $this->params['options'] = empty($this->params['options']) ?
          array() : json_decode(htmlspecialchars_decode(
                        $this->params['options']), TRUE);
    }

    /**
     * Move non-file fields into params array before
     * upload validation and RPC execution
     */
    protected function mergeFields() {
        foreach($this->params as $key => $param) {
            switch($key) {
                case "service":
                case "method":
                case "params":
                case "options":
                    break;
                default:
                    $this->params['params'][$key] = $param;
                    unset($this->params[$key]);
            }
        }
    }

    /**
     * Check the uploaded files. Append the filenames
     * to the RPC service method parameters.
     * @throw E_MISC_FAILURE
     */
    protected function preExecute() {
        // Are the file(s) successfully uploaded?
        foreach($_FILES as $key => $file) {
            if(!in_array($file['error'],
                array(UPLOAD_ERR_OK, UPLOAD_ERR_NO_FILE))) {
                throw new OMVException( OMVErrorMsg::E_MISC_FAILURE,
                    $this->codeToMessage($file['error']));
            }
            if($file['error']===UPLOAD_ERR_OK &&
                !is_uploaded_file($file['tmp_name'])) {
                throw new OMVException( OMVErrorMsg::E_MISC_FAILURE,
                    "Invalid upload request.");
            }
            // Modify the service method parameters.
            if($file['error']===UPLOAD_ERR_OK)
                $this->params['params'][$key] = $file['tmp_name'];
        }
        $this->debug(sprintf("\$this->params array: %s",
                                print_r($this->params, TRUE)));
    }

    /**
     * The function that is called after the RPC has been successfully
     * executed or it has been failed.
     */
    public function cleanup() {
          if($this->secureDeletion === TRUE) {
              $this->debug("Cleanup: shredding temporary files");
              $this->shredFiles();
          } else {
              $this->debug("Cleanup: unlinking temporary files");
              $this->unlinkFiles();
          }
    }

    /**
     * Remove uploaded files using shred.
     */
    protected function shredFiles() {
        foreach($_FILES as $key => $file) {
            $this->debug(sprintf("Deleting file: %s => %s",
                                    $key,
                                    $file['tmp_name']));
            if($file['error']===UPLOAD_ERR_OK &&
                is_uploaded_file($file['tmp_name'])) {
                $cmd = sprintf("export LANG=C; shred -u %s ",
                                escapeshellarg($file['tmp_name']));
                @OMVUtil::exec($cmd, $output, $result);
                if ($result !== 0) {
                    $this->debug($output);
                    // fall back to PHP deletion
                    if(file_exists($file['tmp_name']))
                        unlink($file['tmp_name']);
                }
                unset($cmd, $output, $result);
            }
        }
    }

    /**
     * Remove uploaded files.
     */
    protected function unlinkFiles() {
        foreach($_FILES as $key => $file) {
            $this->debug(sprintf("Deleting file: %s => %s",
                                    $key,
                                    $this->params['params'][$key]));
            if(array_key_exists($key, $this->params['params'])) {
                if(file_exists($this->params['params'][$key]))
                    unlink($this->params['params'][$key]);
            }
        }
    }
}
