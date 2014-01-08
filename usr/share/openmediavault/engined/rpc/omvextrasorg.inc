<?php
/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Aaron Murray <aaron@omv-extras.org>
 * @copyright Copyright (c) 2013-2014 Aaron Murray
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
require_once("openmediavault/object.inc");
require_once("openmediavault/config.inc");
require_once("openmediavault/error.inc");
require_once("openmediavault/util.inc");
require_once("openmediavault/rpcservice.inc");
require_once("openmediavault/notify.inc");

class OMVRpcServiceOmvExtrasOrg extends OMVRpcServiceAbstract {
    /**
     * Get the RPC service name.
     */
    public function getName() {
        return "OmvExtrasOrg";
    }

    /**
     * Initialize the RPC service.
     */
    public function initialize() {
        $this->registerMethod("getPrimary");
        $this->registerMethod("setPrimary");
        $this->registerMethod("getSecondary");
        $this->registerMethod("setSecondary");
        $this->registerMethod("getDeveloper");
        $this->registerMethod("setDeveloper");
        $this->registerMethod("doInstallBackports");
        $this->registerMethod("doAptClean");
    }

    public function getPrimary($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Get the configuration object.
        $xpath = "//system/omvextrasorg";
        $object = $xmlConfig->get($xpath);
        if(is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);

        // Modify the result object.
        $object['enable']   = boolval($object['enable']);
        $object['testing']  = boolval($object['testing']);

        $cmd = sprintf("dpkg-query -W -f='\${Version}' %s", "openmediavault-omvextrasorg");
        OMVUtil::exec($cmd, $output, $result);

        $object['version'] = $output[0];

        $output = array();

        $cmd = "uname -r";
        OMVUtil::exec($cmd, $output, $result);

        $object['kernel'] = $output[0];

        return $object;
    }

    public function setPrimary($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "enable"    : { "type" : "boolean" },
                "testing"   : { "type" : "boolean" },
                "version"   : { "type" : "string", "optional" : true },
                "kernel"    : { "type" : "string", "optional" : true },
                "developer" : { "type" : "string", "optional" : true }
            }
        }');

        // Get the existing configuration object.
        $xpath = "//system/omvextrasorg";
        $object = $xmlConfig->get($xpath);
        if(is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);

        // Update the configuration object.
        $object = array(
            "enable"        => array_boolval($params, 'enable'),
            "testing"       => array_boolval($params, 'testing'),
            "developer"     => $params['developer'],
            "vbox"          => array_boolval($object, 'vbox'),
            "plex"          => array_boolval($object, 'plex'),
            "greyhole"      => array_boolval($object, 'greyhole'),
            "vdr"           => array_boolval($object, 'vdr'),
            "miller"        => array_boolval($object, 'miller'),
            "millertesting" => array_boolval($object, 'millertesting'),
            "btsync"        => array_boolval($object, 'btsync'),
            "beta"          => array_boolval($object, 'beta'),
            "dotdeb"        => array_boolval($object, 'dotdeb'),
            "debmm"         => array_boolval($object, 'debmm'),
            "debmmbp"       => array_boolval($object, 'debmmbp')
        );
        if(FALSE === $xmlConfig->replace("//system/omvextrasorg", $object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED);

        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY,
            "org.openmediavault.system.omvextrasorg", $object);

        // Apply the changes immediately (no user interaction is wanted).
        OMVRpc::exec("Config", "applyChanges", array(
            "modules" => array("omvextrasorg"),
            "force"   => TRUE
        ), $context);

        // Return the configuration object.
        return $object;
    }

    public function getSecondary($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Get the configuration object.
        $xpath = "//system/omvextrasorg";
        $object = $xmlConfig->get($xpath);
        if(is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);

        // Modify the result object.
        $object['vbox']          = boolval($object['vbox']);
        $object['plex']          = boolval($object['plex']);
        $object['greyhole']      = boolval($object['greyhole']);
        $object['vdr']           = boolval($object['vdr']);
        $object['miller']        = boolval($object['miller']);
        $object['millertesting'] = boolval($object['millertesting']);
        $object['btsync']        = boolval($object['btsync']);

        return $object;
    }

    public function setSecondary($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "vbox"          : { "type" : "boolean" },
                "plex"          : { "type" : "boolean" },
                "greyhole"      : { "type" : "boolean" },
                "vdr"           : { "type" : "boolean" },
                "miller"        : { "type" : "boolean" },
                "millertesting" : { "type" : "boolean" },
                "btsync"        : { "type" : "boolean" }
            }
        }');

        $uname = php_uname();

        if ( strpos($uname, "x86") === false && strpos($uname, "486") === false && strpos($uname, "686") === false ) {
            $params['vbox']     = false;
            $params['plex']     = false;
            $params['greyhole'] = false;
            $params['vdr']      = false;
        }

        // Get the existing configuration object.
        $xpath = "//system/omvextrasorg";
        $object = $xmlConfig->get($xpath);
        if(is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);

        // Update the configuration object.
        $object = array(
            "enable"        => array_boolval($object, 'enable'),
            "testing"       => array_boolval($object, 'testing'),
            "developer"     => $object['developer'],
            "vbox"          => array_boolval($params, 'vbox'),
            "plex"          => array_boolval($params, 'plex'),
            "greyhole"      => array_boolval($params, 'greyhole'),
            "vdr"           => array_boolval($params, 'vdr'),
            "miller"        => array_boolval($params, 'miller'),
            "millertesting" => array_boolval($params, 'millertesting'),
            "btsync"        => array_boolval($params, 'btsync'),
            "beta"          => array_boolval($object, 'beta'),
            "dotdeb"        => array_boolval($object, 'dotdeb'),
            "debmm"         => array_boolval($object, 'debmm'),
            "debmmbp"       => array_boolval($object, 'debmmbp')
        );

        if(FALSE === $xmlConfig->replace("//system/omvextrasorg", $object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED);

        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY,
            "org.openmediavault.system.omvextrasorg", $object);

        // Apply the changes immediately (no user interaction is wanted).
        OMVRpc::exec("Config", "applyChanges", array(
            "modules" => array("omvextrasorg"),
            "force"   => TRUE
        ), $context);

        // Return the configuration object.
        return $object;
    }

    public function getDeveloper($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Get the configuration object.
        $xpath = "//system/omvextrasorg";
        $object = $xmlConfig->get($xpath);
        if(is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);

        // Modify the result object.
        $object['beta']    = boolval($object['beta']);
        $object['dotdeb']  = boolval($object['dotdeb']);
        $object['debmm']   = boolval($object['debmm']);
        $object['debmmbp'] = boolval($object['debmmbp']);

        return $object;
    }

    public function setDeveloper($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "beta"    : { "type" : "boolean" },
                "dotdeb"  : { "type" : "boolean" },
                "debmm"   : { "type" : "boolean" },
                "debmmbp" : { "type" : "boolean" }
            }
        }');

        // Get the existing configuration object.
        $xpath = "//system/omvextrasorg";
        $object = $xmlConfig->get($xpath);
        if(is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);

        // Update the configuration object.
        $object = array(
            "enable"        => array_boolval($object, 'enable'),
            "testing"       => array_boolval($object, 'testing'),
            "developer"     => $object['developer'],
            "vbox"          => array_boolval($object, 'vbox'),
            "plex"          => array_boolval($object, 'plex'),
            "greyhole"      => array_boolval($object, 'greyhole'),
            "vdr"           => array_boolval($object, 'vdr'),
            "miller"        => array_boolval($object, 'miller'),
            "millertesting" => array_boolval($object, 'millertesting'),
            "btsync"        => array_boolval($object, 'btsync'),
            "beta"          => array_boolval($params, 'beta'),
            "dotdeb"        => array_boolval($params, 'dotdeb'),
            "debmm"         => array_boolval($params, 'debmm'),
            "debmmbp"       => array_boolval($params, 'debmmbp')
        );
        if(FALSE === $xmlConfig->replace("//system/omvextrasorg", $object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED);

        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY,
            "org.openmediavault.system.omvextrasorg", $object);

        // Apply the changes immediately (no user interaction is wanted).
        OMVRpc::exec("Config", "applyChanges", array(
            "modules" => array("omvextrasorg"),
            "force"   => TRUE
        ), $context);

        // Return the configuration object.
        return $object;
    }

    function doInstallBackports($params, $context) {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Create a background process.
        $bgStatusFilename = $this->createBgProcStatus();
        $pid = $this->fork();
        if($pid > 0) { // Parent process.
            $this->initializeBgProcStatus($bgStatusFilename, $pid);
            return $bgStatusFilename;
        }

        // Child process.
        try {
            $bgOutputFilename = $this->createBgProcOutput();
            $this->updateBgProcStatus($bgStatusFilename, "outputfilename", $bgOutputFilename);

            $cmd = "/usr/local/bin/installbackports";
            $this->exec($cmd, $output, $bgOutputFilename);
            $this->finalizeBgProcStatus($bgStatusFilename, $output);

        } catch(Exception $e) {
            $this->finalizeBgProcStatus($bgStatusFilename, "", $e);
            exit(1);
        }
    }

    function doAptClean($params, $context) {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        $cmd = "/usr/bin/apt-get clean";
        if(0 !== $this->exec($cmd, $output))
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED, $cmd, implode("\n", $output));

        $cmd = "rm -rf /var/lib/apt/lists/*";
        if(0 !== $this->exec($cmd, $output))
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED, $cmd, implode("\n", $output));

        $cmd = "rm -f /var/cache/openmediavault/archives/*";
        if(0 !== $this->exec($cmd, $output))
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED, $cmd, implode("\n", $output));

        touch("/var/cache/openmediavault/archives/Packages");

        $cmd = "/usr/bin/apt-get update";
        if(0 !== $this->exec($cmd, $output))
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED, $cmd, implode("\n", $output));
    }
}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServiceOmvExtrasOrg());
?>
