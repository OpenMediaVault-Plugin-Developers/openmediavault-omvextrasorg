<?php
/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2014 OpenMediaVault Plugin Developers
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
require_once("openmediavault/product.inc");

class OMVRpcServiceOmvExtrasOrg extends OMVRpcServiceAbstract
{
    public function getName()
    {
        return "OmvExtrasOrg";
    }

    private function getPluginName()
    {
        return strtolower( $this->getName() );
    }

    private function getXPath()
    {
        return sprintf( "//system/%s", $this->getPluginName() );
    }

    private function getXPathRepos()
    {
        return sprintf( "%s/repos", $this->getXPath() );
    }

    private function getXPathRepo()
    {
        return sprintf( "%s/repo", $this->getXPathRepos() );
    }

    private function getXPathModules()
    {
        return sprintf( "%s/modules", $this->getXPath() );
    }

    private function getXPathResetPerms()
    {
        return sprintf( "%s/resetperms", $this->getXPath() );
    }

    private function getEventMessagePath()
    {
        return sprintf( "org.openmediavault.system.%s", $this->getPluginName() );
    }

    private function getEventMessagePathRepo()
    {
        return sprintf( "%s.repos.repo", $this->getEventMessagePath() );
    }

    private function getEventMessagePathModules()
    {
        return sprintf( "%s.modules", $this->getEventMessagePath() );
    }

    private function getEventMessagePathResetPerms()
    {
        return sprintf( "%s.resetperms", $this->getEventMessagePath() );
    }

    private function getSysinfoModuleDir()
    {
        return "/usr/share/openmediavault/sysinfo/modules.d/";
    }

    private function getOmvSupport()
    {
        return "/usr/local/bin/omv-support.sh";
    }

    private function getShowkey()
    {
        return "/usr/local/bin/showkey";
    }

    public function initialize()
    {
        $this->registerMethod("getPrimary");
        $this->registerMethod("setPrimary");

        $this->registerMethod("getSecondary");
        $this->registerMethod("setSecondary");

        $this->registerMethod("getRepos");
        $this->registerMethod("getRepo");
        $this->registerMethod("setRepo");
        $this->registerMethod("deleteRepo");

        $this->registerMethod("getDeveloper");
        $this->registerMethod("setDeveloper");

        $this->registerMethod("getModules");
        $this->registerMethod("setModules");

        $this->registerMethod("getStyles");

        $this->registerMethod("getResetPerms");
        $this->registerMethod("setResetPerms");

        $this->registerMethod("doInstallBackports");
        $this->registerMethod("doInstallSystemd");
        $this->registerMethod("doAptClean");
        $this->registerMethod("doAptCleanSilent");

        $this->registerMethod("getSupportInfo");
        $this->registerMethod("getProcessList");
        $this->registerMethod("doSend");

        $this->registerMethod("doResetPerms");
    }

    public function getPrimary($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Get the configuration object.
        $object = $xmlConfig->get( $this->getXPath() );
        if (is_null($object))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPath()
            );
        }

        // Modify the result object.
        $object['enable']  = boolval($object['enable']);
        $object['testing'] = boolval($object['testing']);

        // Get OMV version name
        $prd = new OMVProduct();
        $object['versionname'] = strtolower( $prd->getVersionName() );

        // Get omv-extras version
        $output = array();
        $cmd = sprintf( "dpkg-query -W -f='\${Version}' openmediavault-%s", $this->getPluginName() );
        OMVUtil::exec($cmd, $output, $result);
        $object['version'] = $output[0];

        // Get kernel version
        $object['kernel'] = php_uname('r');

        // Get architecture
        $output = array();
        $cmd = "dpkg --print-architecture";
        OMVUtil::exec($cmd, $output, $result);
        $object['arch'] = strtolower( $output[0] );

        // Determine if backports kernel button should be shown
        $pos = strpos( $object['arch'], "arm" );
        if ( $pos === false )
            $object['showbackports'] = "1";
        else
            $object['showbackports'] = "0";

        // Remove useless fields from the result object.
        unset($object['repos']);
        unset($object['modules']);
        unset($object['resetperms']);

        return $object;
    }

    public function setPrimary($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "enable"    : { "type" : "boolean" },
                "testing"   : { "type" : "boolean" },
                "developer" : { "type" : "string", "optional" : true }
            }
        }');

        // Get the existing configuration object.
        $oldObject = $xmlConfig->get( $this->getXPath() );
        if (is_null($oldObject))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPath()
            );
        }

        // Update the configuration object.
        $object = array(
            "enable"        => array_boolval($params, 'enable'),
            "testing"       => array_boolval($params, 'testing'),
            "developer"     => $params['developer'],
            "vbox"          => array_boolval($oldObject, 'vbox'),
            "plex"          => array_boolval($oldObject, 'plex'),
            "greyhole"      => array_boolval($oldObject, 'greyhole'),
            "vdr"           => array_boolval($oldObject, 'vdr'),
            "miller"        => array_boolval($oldObject, 'miller'),
            "millertesting" => array_boolval($oldObject, 'millertesting'),
            "btsync"        => array_boolval($oldObject, 'btsync'),
            "vpn"           => array_boolval($oldObject, 'vpn'),
            "zfs"           => array_boolval($oldObject, 'zfs'),
            "gluster"       => array_boolval($oldObject, 'gluster'),
            "mono"          => array_boolval($oldObject, 'mono'),
            "hwraid"        => array_boolval($oldObject, 'hwraid'),
            "beta"          => array_boolval($oldObject, 'beta'),
            "dotdeb"        => array_boolval($oldObject, 'dotdeb'),
            "debmm"         => array_boolval($oldObject, 'debmm'),
            "debmmbp"       => array_boolval($oldObject, 'debmmbp'),
            "mariadb"       => array_boolval($oldObject, 'mariadb'),
            "repos"         => isset($oldObject['repos']) ? $oldObject['repos'] : array(),
            "modules"       => isset($oldObject['modules']) ? $oldObject['modules'] : array(),
            "resetperms"    => isset($oldObject['resetperms']) ? $oldObject['resetperms'] : array()
        );
        if (FALSE === $xmlConfig->replace($this->getXPath(), $object))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED
            );
        }

        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(
            OMV_NOTIFY_MODIFY,
            $this->getEventMessagePath(),
            $object
        );

        // Apply the changes immediately (no user interaction is wanted).
        OMVRpc::exec(
            "Config",
            "applyChanges",
            array(
                "modules" => array( $this->getPluginName() ),
                "force"   => TRUE
            ),
            $context
        );

        // Return the configuration object.
        return $object;
    }

    public function getSecondary($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Get the configuration object.
        $object = $xmlConfig->get( $this->getXPath() );
        if (is_null($object))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPath()
            );
        }

        // Modify the result object.
        $object['vbox']          = boolval($object['vbox']);
        $object['plex']          = boolval($object['plex']);
        $object['greyhole']      = boolval($object['greyhole']);
        $object['vdr']           = boolval($object['vdr']);
        $object['miller']        = boolval($object['miller']);
        $object['millertesting'] = boolval($object['millertesting']);
        $object['btsync']        = boolval($object['btsync']);
        $object['vpn']           = boolval($object['vpn']);
        $object['zfs']           = boolval($object['zfs']);
        $object['gluster']       = boolval($object['gluster']);
        $object['mono']          = boolval($object['mono']);
        $object['hwraid']        = boolval($object['hwraid']);

        // Remove useless fields from the result object.
        unset($object['repos']);
        unset($object['modules']);
        unset($object['resetperms']);

        return $object;
    }

    public function setSecondary($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

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
                "btsync"        : { "type" : "boolean" },
                "vpn"           : { "type" : "boolean" },
                "zfs"           : { "type" : "boolean" },
                "gluster"       : { "type" : "boolean" },
                "mono"          : { "type" : "boolean" },
                "hwraid"        : { "type" : "boolean" }
            }
        }');

        // Check architecture and disable appropriate repos
        $uname = php_uname();

        if ( strpos($uname, "x86") === false &&
             strpos($uname, "486") === false &&
             strpos($uname, "686") === false )
        {
            $params['vbox']   = false;
            $params['plex']   = false;
            $params['vdr']    = false;
            $params['vpn']    = false;
            $params['mono']   = false;
            $params['hwraid'] = false;
        }

        if ( strpos($uname, "amd64") === false )
        {
            $params['zfs'] = false;
            $params['gluster'] = false;
        }

        // Get the existing configuration object.
        $oldObject = $xmlConfig->get( $this->getXPath() );
        if (is_null($oldObject))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPath()
            );
        }

        // Update the configuration object.
        $object = array(
            "enable"        => array_boolval($oldObject, 'enable'),
            "testing"       => array_boolval($oldObject, 'testing'),
            "developer"     => $oldObject['developer'],
            "vbox"          => array_boolval($params, 'vbox'),
            "plex"          => array_boolval($params, 'plex'),
            "greyhole"      => array_boolval($params, 'greyhole'),
            "vdr"           => array_boolval($params, 'vdr'),
            "miller"        => array_boolval($params, 'miller'),
            "millertesting" => array_boolval($params, 'millertesting'),
            "btsync"        => array_boolval($params, 'btsync'),
            "vpn"           => array_boolval($params, 'vpn'),
            "zfs"           => array_boolval($params, 'zfs'),
            "gluster"       => array_boolval($params, 'gluster'),
            "mono"          => array_boolval($params, 'mono'),
            "hwraid"        => array_boolval($params, 'hwraid'),
            "beta"          => array_boolval($oldObject, 'beta'),
            "dotdeb"        => array_boolval($oldObject, 'dotdeb'),
            "debmm"         => array_boolval($oldObject, 'debmm'),
            "debmmbp"       => array_boolval($oldObject, 'debmmbp'),
            "mariadb"       => array_boolval($oldObject, 'mariadb'),
            "repos"         => isset($oldObject['repos']) ? $oldObject['repos'] : array(),
            "modules"       => isset($oldObject['modules']) ? $oldObject['modules'] : array(),
            "resetperms"    => isset($oldObject['resetperms']) ? $oldObject['resetperms'] : array()
        );

        if (FALSE === $xmlConfig->replace($this->getXPath(), $object))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED
            );
        }

        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(
            OMV_NOTIFY_MODIFY,
            $this->getEventMessagePath(),
            $object
        );

        // Apply the changes immediately (no user interaction is wanted).
        OMVRpc::exec(
            "Config",
            "applyChanges",
            array(
                "modules" => array( $this->getPluginName() ),
                "force"   => TRUE
            ),
            $context
        );

        // Return the configuration object.
        return $object;
    }

    public function getRepos($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "start"     : { "type" : "integer" },
                "limit"     : { '.$GLOBALS['OMV_JSONSCHEMA_COUNTFIELD'].' },
                "sortfield" : { '.$GLOBALS['OMV_JSONSCHEMA_SORTFIELD'].' },
                "sortdir"   : { '.$GLOBALS['OMV_JSONSCHEMA_SORTDIR'].' }
            }
        }');

        // Get configuration data.
        $objects = $xmlConfig->getList( $this->getXPathRepo() );
        if (is_null($objects))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPathRepo()
            );
        }

        // Filter the result.
        return $this->applyFilter($objects, $params['start'], $params['limit'],
            $params['sortfield'], $params['sortdir']);
    }

    public function getRepo($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID'].' }
            }
        }');

        // Get the configuration object.
        $xPath = sprintf("%s[uuid='%s']", $this->getXPathRepo(), $params['uuid']);
        $object = $xmlConfig->get($xPath);
        if (is_null($object))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $xPath
            );
        }

        return $object;
    }

    public function setRepo($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid"    : { '.$GLOBALS['OMV_JSONSCHEMA_UUID_UNDEFINED'].' },
                "name"    : { "type" : "string" },
                "repo"    : { "type" : "string" },
                "comment" : { "type" : "string" }
            }
        }');

        // Prepare configuration data.
        $object = array(
            "uuid"    => ($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ? OMVUtil::uuid() : $params['uuid'],
            "name"    => $params['name'],
            "repo"    => $params['repo'],
            "comment" => $params['comment']
        );

        // Set the configuration data.
        $success = FALSE;
        if ($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED'])
        {
            // Append object to configuration.
            $success = $xmlConfig->set($this->getXPathRepos(), array("repo" => $object));
        }
        else
        {
            // Update existing configuration object.
            $xPath = sprintf("%s[uuid='%s']", $this->getXPathRepo(), $params['uuid']);
            $success = $xmlConfig->replace($xPath, $object);
        }

        if (FALSE === $success)
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED
            );
        }

        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(
            ($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ? OMV_NOTIFY_CREATE : OMV_NOTIFY_MODIFY,
            $this->getEventMessagePathRepo(),
            $object
        );

        // Return the configuration object.
        return $object;
    }

    public function deleteRepo($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID'].' }
            }
        }');

        // Delete the configuration object.
        $xPath = sprintf("%s[uuid='%s']", $this->getXPathRepo(), $params['uuid']);
        return $this->deleteConfigObjectByPath($xPath, $this->getEventMessagePathRepo());
    }

    public function getDeveloper($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Get the configuration object.
        $object = $xmlConfig->get( $this->getXPath() );
        if (is_null($object))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPath()
            );
        }

        // Modify the result object.
        $object['beta']    = boolval($object['beta']);
        $object['dotdeb']  = boolval($object['dotdeb']);
        $object['debmm']   = boolval($object['debmm']);
        $object['debmmbp'] = boolval($object['debmmbp']);
        $object['mariadb'] = boolval($object['mariadb']);

        // Remove useless fields from the result object.
        unset($object['repos']);
        unset($object['modules']);
        unset($object['resetperms']);

        return $object;
    }

    public function setDeveloper($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "beta"        : { "type" : "boolean" },
                "dotdeb"      : { "type" : "boolean" },
                "debmm"       : { "type" : "boolean" },
                "debmmbp"     : { "type" : "boolean" },
                "mariadb"     : { "type" : "boolean" }
            }
        }');

        // Get the existing configuration object.
        $oldObject = $xmlConfig->get( $this->getXPath() );
        if (is_null($oldObject))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPath()
            );
        }

        // Update the configuration object.
        $object = array(
            "enable"        => array_boolval($oldObject, 'enable'),
            "testing"       => array_boolval($oldObject, 'testing'),
            "developer"     => $oldObject['developer'],
            "vbox"          => array_boolval($oldObject, 'vbox'),
            "plex"          => array_boolval($oldObject, 'plex'),
            "greyhole"      => array_boolval($oldObject, 'greyhole'),
            "vdr"           => array_boolval($oldObject, 'vdr'),
            "miller"        => array_boolval($oldObject, 'miller'),
            "millertesting" => array_boolval($oldObject, 'millertesting'),
            "btsync"        => array_boolval($oldObject, 'btsync'),
            "vpn"           => array_boolval($oldObject, 'vpn'),
            "zfs"           => array_boolval($oldObject, 'zfs'),
            "gluster"       => array_boolval($oldObject, 'gluster'),
            "mono"          => array_boolval($oldObject, 'mono'),
            "hwraid"        => array_boolval($oldObject, 'hwraid'),
            "beta"          => array_boolval($params, 'beta'),
            "dotdeb"        => array_boolval($params, 'dotdeb'),
            "debmm"         => array_boolval($params, 'debmm'),
            "debmmbp"       => array_boolval($params, 'debmmbp'),
            "mariadb"       => array_boolval($params, 'mariadb'),
            "repos"         => isset($oldObject['repos']) ? $oldObject['repos'] : array(),
            "modules"       => isset($oldObject['modules']) ? $oldObject['modules'] : array(),
            "resetperms"    => isset($oldObject['resetperms']) ? $oldObject['resetperms'] : array()
        );

        if (FALSE === $xmlConfig->replace($this->getXPath(), $object))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED
            );
        }

        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(
            OMV_NOTIFY_MODIFY,
            $this->getEventMessagePath(),
            $object
        );

        // Apply the changes immediately (no user interaction is wanted).
        OMVRpc::exec(
            "Config",
            "applyChanges",
            array(
                "modules" => array( $this->getPluginName() ),
                "force"   => TRUE
            ),
            $context
        );

        // Return the configuration object.
        return $object;
    }

    public function getModules($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Get the configuration object.
        $object = $xmlConfig->get( $this->getXPathModules() );
        if (is_null($object))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPathModules()
            );
        }

        // Modify the result object.
        $object['debian-version'] = boolval($object['debian-version']);
        $object['omv-version']    = boolval($object['omv-version']);
        $object['date']           = boolval($object['date']);
        $object['hostname']       = boolval($object['hostname']);
        $object['locale']         = boolval($object['locale']);
        $object['uname']          = boolval($object['uname']);
        $object['uptime']         = boolval($object['uptime']);
        $object['interfaces']     = boolval($object['interfaces']);
        $object['iptables']       = boolval($object['iptables']);
        $object['netstat']        = boolval($object['netstat']);
        $object['route']          = boolval($object['route']);
        $object['mdadm']          = boolval($object['mdadm']);
        $object['monit']          = boolval($object['monit']);
        $object['hpraid']         = boolval($object['hpraid']);
        $object['blkid']          = boolval($object['blkid']);
        $object['df']             = boolval($object['df']);
        $object['fstab']          = boolval($object['fstab']);
        $object['mountinfo']      = boolval($object['mountinfo']);
        $object['partitions']     = boolval($object['partitions']);
        $object['apt-sources']    = boolval($object['apt-sources']);
        $object['dpkg']           = boolval($object['dpkg']);
        $object['omv-plugins']    = boolval($object['omv-plugins']);
        $object['lsmod']          = boolval($object['lsmod']);

        return $object;
    }

    public function setModules($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "debian-version" : { "type" : "boolean" },
                "omv-version"    : { "type" : "boolean" },
                "date"           : { "type" : "boolean" },
                "hostname"       : { "type" : "boolean" },
                "locale"         : { "type" : "boolean" },
                "uname"          : { "type" : "boolean" },
                "uptime"         : { "type" : "boolean" },
                "interfaces"     : { "type" : "boolean" },
                "iptables"       : { "type" : "boolean" },
                "netstat"        : { "type" : "boolean" },
                "route"          : { "type" : "boolean" },
                "mdadm"          : { "type" : "boolean" },
                "monit"          : { "type" : "boolean" },
                "hpraid"         : { "type" : "boolean" },
                "blkid"          : { "type" : "boolean" },
                "df"             : { "type" : "boolean" },
                "fstab"          : { "type" : "boolean" },
                "mountinfo"      : { "type" : "boolean" },
                "partitions"     : { "type" : "boolean" },
                "apt-sources"    : { "type" : "boolean" },
                "dpkg"           : { "type" : "boolean" },
                "omv-plugins"    : { "type" : "boolean" },
                "lsmod"          : { "type" : "boolean" }
            }
        }');

        // Get the existing configuration object.
        $oldObject = $xmlConfig->get( $this->getXPathModules() );
        if (is_null($oldObject))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPathModules()
            );
        }

        // Update the configuration object.
        $object = array(
            "debian-version" => array_boolval($params, 'debian-version'),
            "omv-version"    => array_boolval($params, 'omv-version'),
            "date"           => array_boolval($params, 'date'),
            "hostname"       => array_boolval($params, 'hostname'),
            "locale"         => array_boolval($params, 'locale'),
            "uname"          => array_boolval($params, 'uname'),
            "uptime"         => array_boolval($params, 'uptime'),
            "interfaces"     => array_boolval($params, 'interfaces'),
            "iptables"       => array_boolval($params, 'iptables'),
            "netstat"        => array_boolval($params, 'netstat'),
            "route"          => array_boolval($params, 'route'),
            "mdadm"          => array_boolval($params, 'mdadm'),
            "monit"          => array_boolval($params, 'monit'),
            "hpraid"         => array_boolval($params, 'hpraid'),
            "blkid"          => array_boolval($params, 'blkid'),
            "df"             => array_boolval($params, 'df'),
            "fstab"          => array_boolval($params, 'fstab'),
            "mountinfo"      => array_boolval($params, 'mountinfo'),
            "partitions"     => array_boolval($params, 'partitions'),
            "apt-sources"    => array_boolval($params, 'apt-sources'),
            "dpkg"           => array_boolval($params, 'dpkg'),
            "omv-plugins"    => array_boolval($params, 'omv-plugins'),
            "lsmod"          => array_boolval($params, 'lsmod')
        );
        if (FALSE === $xmlConfig->replace($this->getXPathModules(), $object))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED
            );
        }

        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(
            OMV_NOTIFY_MODIFY,
            $this->getEventMessagePathModules(),
            $object
        );

        // Return the configuration object.
        return $object;
    }

    public function getStyles($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "start"     : { "type" : "integer" },
                "limit"     : { '.$GLOBALS['OMV_JSONSCHEMA_COUNTFIELD'].' },
                "sortfield" : { '.$GLOBALS['OMV_JSONSCHEMA_SORTFIELD'].' },
                "sortdir"   : { '.$GLOBALS['OMV_JSONSCHEMA_SORTDIR'].' }
            }
        }');

        // Get configuration object
        $styles = array();

        $styles[] = array(
            "cmd"  => "style1",
            "name" => "Style #1"
        );

        $styles[] = array(
            "cmd"  => "style2",
            "name" => "Style #2"
        );

        // Get configuration data.
        $cmd = "export LANG=C; dpkg --get-selections | grep openmediavault-";
        OMVUtil::exec($cmd, $result);

        foreach ($result as $style)
        {
            $pluginTitle = trim(str_replace("install", "", $style));
            $pluginTitle = trim(str_replace("openmediavault-", "", $pluginTitle));

            if ($pluginTitle == "keyring")
                continue;

            $styles[] = array(
                "cmd"  => $pluginTitle,
                "name" => sprintf("XML - %s", $pluginTitle)
            );
        }

        return $this->applyFilter($styles, $params["start"], $params["limit"],
            $params["sortfield"], $params["sortdir"]
        );
    }

    public function getResetPerms($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Get the configuration object.
        $object = $xmlConfig->get( $this->getXPathResetPerms() );
        if (is_null($object))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPathResetPerms()
            );
        }

        // Modify the result object.
        $object['clearacl'] = boolval($object['clearacl']);

        return $object;
    }

    public function setResetPerms($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "sharedfolderref" : { ' . $GLOBALS["OMV_JSONSCHEMA_UUID"] . ' },
                "mode"            : { "type" : "string" },
                "clearacl"        : { "type" : "boolean" }
            }
        }');

        // Get the existing configuration object.
        $oldObject = $xmlConfig->get( $this->getXPathResetPerms() );
        if (is_null($oldObject))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPathResetPerms()
            );
        }

        // Update the configuration object.
        $object = array(
            "sharedfolderref" => $params["sharedfolderref"],
            "mode"            => $params['mode'],
            "clearacl"        => array_boolval($params, 'clearacl')
        );
        if (FALSE === $xmlConfig->replace($this->getXPathResetPerms(), $object))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED
            );
        }

        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(
            OMV_NOTIFY_MODIFY,
            $this->getEventMessagePathResetPerms(),
            $object
        );

        // Return the configuration object.
        return $object;
    }

    public function doInstallBackports($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Create a background process.
        $bgStatusFilename = $this->createBgProcStatus();
        $pid = $this->fork();
        if ($pid > 0)   // Parent process.
        {
            $this->initializeBgProcStatus($bgStatusFilename, $pid);
            return $bgStatusFilename;
        }

        // Child process.
        try {
            $bgOutputFilename = $this->createBgProcOutput();
            $this->updateBgProcStatus($bgStatusFilename, "outputfilename", $bgOutputFilename);

            $cmd = sprintf("omv-mkconf %s installbackports 2>&1", $this->getPluginName() );
            $this->exec($cmd, $output, $bgOutputFilename);
            $this->finalizeBgProcStatus($bgStatusFilename, $output);

        } catch(Exception $e) {
            $this->finalizeBgProcStatus($bgStatusFilename, "", $e);
            exit(1);
        }
    }

    public function doInstallSystemd($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Create a background process.
        $bgStatusFilename = $this->createBgProcStatus();
        $pid = $this->fork();
        if ($pid > 0)   // Parent process.
        {
            $this->initializeBgProcStatus($bgStatusFilename, $pid);
            return $bgStatusFilename;
        }

        // Child process.
        try {
            $bgOutputFilename = $this->createBgProcOutput();
            $this->updateBgProcStatus($bgStatusFilename, "outputfilename", $bgOutputFilename);

            $cmd = sprintf("omv-mkconf %s installsystemd 2>&1", $this->getPluginName() );
            $this->exec($cmd, $output, $bgOutputFilename);
            $this->finalizeBgProcStatus($bgStatusFilename, $output);

        } catch(Exception $e) {
            $this->finalizeBgProcStatus($bgStatusFilename, "", $e);
            exit(1);
        }
    }

    public function doAptCleanSilent($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        $cmd = sprintf("omv-mkconf %s aptclean 2>&1", $this->getPluginName() );
        if (0 !== $this->exec($cmd, $output))
        {
            throw new OMVException(
                OMVErrorMsg::E_EXEC_FAILED,
                $cmd,
                implode("\n", $output)
            );
        }
    }

    public function doAptClean($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Create a background process.
        $bgStatusFilename = $this->createBgProcStatus();
        $pid = $this->fork();
        if ($pid > 0)   // Parent process.
        {
            $this->initializeBgProcStatus($bgStatusFilename, $pid);
            return $bgStatusFilename;
        }

        // Child process.
        try {
            $bgOutputFilename = $this->createBgProcOutput();
            $this->updateBgProcStatus($bgStatusFilename, "outputfilename", $bgOutputFilename);

            $cmd = sprintf("omv-mkconf %s aptclean 2>&1", $this->getPluginName() );
            $this->exec($cmd, $output, $bgOutputFilename);
            $this->finalizeBgProcStatus($bgStatusFilename, $output);

        } catch(Exception $e) {
            $this->finalizeBgProcStatus($bgStatusFilename, "", $e);
            exit(1);
        }
    }

    public function getSupportInfo($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "info" : { "type" : "string", "optional" : true }
            }
        }');

        $output = "";
        $result = "";

        switch ($params['info'])
        {
            case "":
            case "style1":
                // Get the configuration object.
                $object = $xmlConfig->get( $this->getXPathModules() );
                if (is_null($object))
                {
                    throw new OMVException(
                        OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                        $this->getXPathModules()
                    );
                }

                $modules = array(
                    "debian-version","omv-version",
                    "date","hostname","locale","uname","uptime",
                    "interfaces","iptables","netstat","route",
                    "mdadm","monit","hpraid",
                    "blkid","df","fstab","mountinfo","partitions",
                    "apt-sources","dpkg","omv-plugins",
                    "lsmod"
                );

                $sections = array(
                    "00","00",
                    "10","10","10","10","10",
                    "20","20","20","20",
                    "30","30","31",
                    "40","40","40","40","40",
                    "50","50","50",
                    "61"
                );

                foreach ($modules as $modulek => $modulev)
                {
                    if ( boolval($object[$modulev]) === true )
                    {
                        $path = sprintf("%s%s-%s", $this->getSysinfoModuleDir(), $sections[$modulek], $modulev);
                        OMVUtil::exec($path, $result);
                    }
                }
                break;

            case "style2":
                $cmd = sprintf("%s webgui", $this->getOmvSupport() );
                OMVUtil::exec($cmd, $result);
                break;

            default:
                $cmd = sprintf("%s %s", $this->getShowkey(), $params['info']);
                OMVUtil::exec($cmd, $result);
        }
        $output = implode("\n", $result) . "\n";

        return $output;
    }

    public function getProcessList($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Get ps output
        $cmd = 'ps axww -o pid,user,%cpu,%mem,stat,args 2>&1';
        OMVUtil::exec($cmd, $output, $result);

        if ($result !== 0)
        {
            throw new OMVException(
                OMVErrorMsg::E_EXEC_FAILED,
                $cmd,
                implode("\n", $output)
            );
        }

        // Get column names
        $cols = array_map(function($a)
        {
            return preg_replace("/[^A-Z]+/", '', $a);
        }, preg_split('/\s+/', trim(array_shift($output))));

        $split_limit = count($cols);

        $objects = array();
        foreach ($output as $line)
        {
            if (trim($line) == '')
                continue;

            $objects[] = array_combine($cols, preg_split('/\s+/', trim($line), $split_limit));
        }

        return array(
            "total" => count($objects),
            "data"  => $objects
        );
    }

    public function doSend($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Create a background process.
        $bgStatusFilename = $this->createBgProcStatus();
        $pid = $this->fork();
        if ($pid > 0)   // Parent process.
        {
            $this->initializeBgProcStatus($bgStatusFilename, $pid);
            return $bgStatusFilename;
        }

        // Child process.
        try {
            $bgOutputFilename = $this->createBgProcOutput();
            $this->updateBgProcStatus($bgStatusFilename, "outputfilename", $bgOutputFilename);

            $cmd = sprintf("omv-mkconf %s sendsupport 2>&1", $this->getPluginName() );
            $this->exec($cmd, $output, $bgOutputFilename);
            $this->finalizeBgProcStatus($bgStatusFilename, $output);

        } catch(Exception $e) {
            $this->finalizeBgProcStatus($bgStatusFilename, "", $e);
            exit(1);
        }
    }

    public function doResetPerms($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Create a background process.
        $bgStatusFilename = $this->createBgProcStatus();
        $pid = $this->fork();
        if ($pid > 0)   // Parent process.
        {
            $this->initializeBgProcStatus($bgStatusFilename, $pid);
            return $bgStatusFilename;
        }

        // Child process.
        try {
            $bgOutputFilename = $this->createBgProcOutput();
            $this->updateBgProcStatus($bgStatusFilename, "outputfilename", $bgOutputFilename);

            $cmd = sprintf("omv-mkconf %s resetperms 2>&1", $this->getPluginName() );
            $this->exec($cmd, $output, $bgOutputFilename);
            $this->finalizeBgProcStatus($bgStatusFilename, $output);

        } catch(Exception $e) {
            $this->finalizeBgProcStatus($bgStatusFilename, "", $e);
            exit(1);
        }
    }
}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServiceOmvExtrasOrg());
