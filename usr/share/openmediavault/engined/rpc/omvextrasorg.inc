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
        return "/usr/local/bin/omv-showkey";
    }

    public function initialize()
    {
        $this->registerMethod("getRepo");
        $this->registerMethod("setRepo");
        $this->registerMethod("getArch");

        $this->registerMethod("getCustomList");
        $this->registerMethod("getCustom");
        $this->registerMethod("setCustom");
        $this->registerMethod("deleteCustom");

        $this->registerMethod("getModules");
        $this->registerMethod("setModules");

        $this->registerMethod("getStyles");

        $this->registerMethod("getResetPerms");
        $this->registerMethod("setResetPerms");

        $this->registerMethod("getKernel");
        $this->registerMethod("setKernel");
        $this->registerMethod("getKernelList");
        $this->registerMethod("setBootKernel");

        $this->registerMethod("doInstallBackports");
        $this->registerMethod("doAptClean");

        $this->registerMethod("getSupportInfo");
        $this->registerMethod("getProcessList");
        $this->registerMethod("doSend");

        $this->registerMethod("doResetPerms");
    }

    public function getRepo($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Get the existing configuration object.
        $object = $xmlConfig->get( $this->getXPath() );
        if (is_null($object))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPath()
            );
        }

        // Remove useless fields from the result object.
        unset($object['repos']);
        unset($object['modules']);
        unset($object['resetperms']);

        // Create repo array
        $result = array();

        $result[] = array(
            "id"          => "btsync",
            "type"        => "Backup",
            "enable"      => $object['btsync'],
            "repo"        => "BTSync",
            "description" => "BTSync"
        );

        $result[] = array(
            "id"          => "gluster",
            "type"        => "Filesystems",
            "enable"      => $object['gluster'],
            "repo"        => "GlusterFS",
            "description" => "Gluster - do not enable this repository unless you are using the GlusterFS filesystem (uncommon)"
        );

        $result[] = array(
            "id"          => "greyhole",
            "type"        => "Filesystems",
            "enable"      => $object['greyhole'],
            "repo"        => "Greyhole",
            "description" => "Greyhole"
        );

        $result[] = array(
            "id"          => "hwraid",
            "type"        => "Utilities",
            "enable"      => $object['hwraid'],
            "repo"        => "HWRaid",
            "description" => "hwraid"
        );

        $result[] = array(
            "id"          => "miller",
            "type"        => "Downloaders",
            "enable"      => $object['miller'],
            "repo"        => "Download Managers",
            "description" => "Download managers - contains various download managers for newsgroups, torrents, usenet, and streaming"
        );

        $result[] = array(
            "id"          => "millertesting",
            "type"        => "Downloaders",
            "enable"      => $object['millertesting'],
            "repo"        => "Download Managers Testing",
            "description" => "Download managers testing - contains various download managers for newsgroups, torrents, usenet, and streaming"
        );

        $result[] = array(
            "id"          => "mono",
            "type"        => "Multimedia",
            "enable"      => $object['mono'],
            "repo"        => "Mono",
            "description" => "Mono and mono-project.com"
        );

        $result[] = array(
            "id"          => "monotesting",
            "type"        => "Multimedia",
            "enable"      => $object['monotesting'],
            "repo"        => "Mono Testing",
            "description" => "Mono testing"
        );

        $result[] = array(
            "id"          => "enable",
            "type"        => "All",
            "enable"      => $object['enable'],
            "repo"        => "OMV-Extras.org",
            "description" => "OMV-Extras"
        );

        $result[] = array(
            "id"          => "testing",
            "type"        => "All",
            "enable"      => $object['testing'],
            "repo"        => "OMV-Extras.org Testing",
            "description" => "OMV-Extras.org testing (release candidates)"
        );

        $result[] = array(
            "id"          => "plex",
            "type"        => "Multimedia",
            "enable"      => $object['plex'],
            "repo"        => "Plex",
            "description" => "Plex Media Server"
        );

        $result[] = array(
            "id"          => "vbox",
            "type"        => "Virtualization",
            "enable"      => $object['vbox'],
            "repo"        => "Virtualbox",
            "description" => "Virtualbox"
        );

        $result[] = array(
            "id"          => "vdr",
            "type"        => "Multimedia",
            "enable"      => $object['vdr'],
            "repo"        => "VDR",
            "description" => "e-tobi.net"
        );

        $result[] = array(
            "id"          => "vpn",
            "type"        => "Remote",
            "enable"      => $object['vpn'],
            "repo"        => "VPN",
            "description" => "VPN - contains plugins for setting up virtual private networks"
        );

        $result[] = array(
            "id"          => "zfs",
            "type"        => "Filesystems",
            "enable"      => $object['zfs'],
            "repo"        => "ZFS",
            "description" => "ZFS - do not enable this repository unless you are using the ZFS filesystem (uncommon)"
        );

        if ( $object['developer'] == "0827" )
        {
            $result[] = array(
                "id"          => "beta",
                "type"        => "All",
                "enable"      => $object['beta'],
                "repo"        => "Beta",
                "description" => "Beta"
            );

            $result[] = array(
                "id"          => "dotdeb",
                "type"        => "All",
                "enable"      => $object['dotdeb'],
                "repo"        => "Dotdeb",
                "description" => "Dotdeb"
            );

            $result[] = array(
                "id"          => "debmm",
                "type"        => "Multimedia",
                "enable"      => $object['debmm'],
                "repo"        => "Deb-Multimedia",
                "description" => "Deb-Multimedia"
            );

            $result[] = array(
                "id"          => "debmmbp",
                "type"        => "Multimedia",
                "enable"      => $object['debmmbp'],
                "repo"        => "Deb-Multimedia Backports",
                "description" => "Deb-Multimedia Backports"
            );

            $result[] = array(
                "id"          => "mariadb",
                "type"        => "Network",
                "enable"      => $object['mariadb'],
                "repo"        => "MariaDB",
                "description" => "MariaDB 10.0"
            );
        }

        return $result;
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
            "type"  : "array",
            "items" : {
                "type"       : "object",
                "properties" : {
                    "id"      : { "type" : "string" },
                    "enable"  : { "type" : "boolean" }
                }
            }
        }');

        // Prepare the configuration object.
        $repos = array();

        foreach($params as $paramk => $paramv)
        {
            $repos[$paramv['id']] = $paramv['enable'];
        }

        // Check architecture and disable appropriate repos
        $uname = php_uname();

        if ( strpos($uname, "x86") === false &&
             strpos($uname, "486") === false &&
             strpos($uname, "686") === false )
        {
            $repos['vbox']        = false;
            $repos['plex']        = false;
            $repos['vdr']         = false;
            $repos['vpn']         = false;
            $repos['mono']        = false;
            $repos['monotesting'] = false;
            $repos['hwraid']      = false;
        }

        if ( boolval($repos['monotesting']) === true )
            $repos['mono'] = true;

        if ( strpos($uname, "amd64") === false )
        {
            $repos['zfs'] = false;
            $repos['gluster'] = false;
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
            "enable"        => array_boolval($repos, 'enable'),
            "testing"       => array_boolval($repos, 'testing'),
            "vbox"          => array_boolval($repos, 'vbox'),
            "plex"          => array_boolval($repos, 'plex'),
            "greyhole"      => array_boolval($repos, 'greyhole'),
            "vdr"           => array_boolval($repos, 'vdr'),
            "miller"        => array_boolval($repos, 'miller'),
            "millertesting" => array_boolval($repos, 'millertesting'),
            "btsync"        => array_boolval($repos, 'btsync'),
            "vpn"           => array_boolval($repos, 'vpn'),
            "zfs"           => array_boolval($repos, 'zfs'),
            "gluster"       => array_boolval($repos, 'gluster'),
            "mono"          => array_boolval($repos, 'mono'),
            "monotesting"   => array_boolval($repos, 'monotesting'),
            "hwraid"        => array_boolval($repos, 'hwraid'),
            "beta"          => array_boolval($repos, 'beta'),
            "dotdeb"        => array_boolval($repos, 'dotdeb'),
            "debmm"         => array_boolval($repos, 'debmm'),
            "debmmbp"       => array_boolval($repos, 'debmmbp'),
            "mariadb"       => array_boolval($repos, 'mariadb'),
            "developer"     => $oldObject['developer'],
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

        // Return the configuration object.
        return $object;
    }

    public function getArch($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Get architecture
        $output = array();
        $cmd = "export LANG=C; dpkg --print-architecture";
        OMVUtil::exec($cmd, $output, $result);
        $object['arch'] = strtolower( $output[0] );

        return $object;
    }

    public function getCustomList($params, $context)
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

    public function getCustom($params, $context)
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

    public function setCustom($params, $context)
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

    public function deleteCustom($params, $context)
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

        $styles[] = array(
            "cmd"  => "iface",
            "name" => "System - iface"
        );

        $styles[] = array(
            "cmd"  => "mntent",
            "name" => "System - mntent"
        );

        $styles[] = array(
            "cmd"  => "sharedfolder",
            "name" => "System - sharedfolder"
        );

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
                "sharedfolderref" : { ' . $GLOBALS["OMV_JSONSCHEMA_UUID_NONE"] . ' },
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

        switch ($params['mode'])
        {
            case "700":
                $params['fileperms'] = "600";
                break;
            case "750":
                $params['fileperms'] = "640";
                break;
            case "770":
                $params['fileperms'] = "660";
                break;
            case "755":
                $params['fileperms'] = "644";
                break;
            case "775":
                $params['fileperms'] = "664";
                break;
            case "777":
                $params['fileperms'] = "666";
                break;
            default:
                $params['fileperms'] = "644";
        }

        $params['dirperms'] = sprintf("2%s", $params['mode']);

        // Update the configuration object.
        $object = array(
            "sharedfolderref" => $params["sharedfolderref"],
            "mode"            => $params['mode'],
            "clearacl"        => array_boolval($params, 'clearacl'),
            "dirperms"        => $params['dirperms'],
            "fileperms"       => $params['fileperms']
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

    public function getKernel($params, $context)
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

        // Get boot kernel
        $grub_default = "/etc/default/grub";
        $line_grub = "GRUB_DEFAULT=";

        $cmd = sprintf("export LANG=C; cat %s | grep %s", $grub_default, $line_grub);
        OMVUtil::exec($cmd, $output, $result);
        $object['kernels'] = intval(str_replace($line_grub, "", $output[0]));

        // Get OMV version name
        $prd = new OMVProduct();
        $object['versionname'] = strtolower( $prd->getVersionName() );

        // Get omv-extras version
        $output = array();
        $cmd = sprintf( "export LANG=C; dpkg-query -W -f='\${Version}' openmediavault-%s", $this->getPluginName() );
        OMVUtil::exec($cmd, $output, $result);
        $object['version'] = $output[0];

        // Get kernel version
        $object['kernel'] = php_uname('r');

        // Get architecture
        $output = array();
        $cmd = "export LANG=C; dpkg --print-architecture";
        OMVUtil::exec($cmd, $output, $result);
        $object['arch'] = strtolower( $output[0] );

        // Remove useless fields from the result object.
        unset($object['repos']);
        unset($object['modules']);
        unset($object['resetperms']);

        return $object;
    }

    public function setKernel($params, $context)
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
                "kernels"   : { "type" : "integer" },
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
            "developer"     => $params['developer'],
            "enable"        => array_boolval($oldObject, 'enable'),
            "testing"       => array_boolval($oldObject, 'testing'),
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
            "monotesting"   => array_boolval($oldObject, 'monotesting'),
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
    }

    public function getKernelList($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Get configuration object
        $kernels = array();

        // Get configuration data.
        $cmd = "export LANG=C; cat /boot/grub/grub.cfg | grep ^menuentry";
        OMVUtil::exec($cmd, $result);

        foreach ($result as $kernelk => $kernelv)
        {
            $line = str_replace("'", "\"", $kernelv);
            $name = explode("\"", $line);

            if (empty($name[1]))
                continue;

            $kernels[] = array(
                "key"  => $kernelk,
                "name" => $name[1]
            );
        }

        return $this->applyFilter($kernels, $params["start"], $params["limit"],
            $params["sortfield"], $params["sortdir"]
        );
    }

    public function setBootKernel($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "key"       : { "type" : "integer" },
                "developer" : { "type" : "string", "optional" : true }
            }
        }');

        // Set boot kernel
        $grub = "/etc/default/grub";
        $line = "GRUB_DEFAULT=";
        $contents = file($grub);
        $newdata = array();
        $new = sprintf("%s%s\n", $line, $params['key']);

        foreach ($contents as $filerow)
        {
            if (strstr($filerow, $line) !== false)
                $filerow = $new;

            $newdata[] = $filerow;
        }

        file_put_contents($grub, implode($newdata));

        $cmd = sprintf("export LANG=C; grub-set-default %s", $params['key']);
        if (0 !== $this->exec($cmd, $output))
        {
            throw new OMVException(
                OMVErrorMsg::E_EXEC_FAILED,
                $cmd,
                implode("\n", $output)
            );
        }

        $cmd = "export LANG=C; update-grub";
        if (0 !== $this->exec($cmd, $output))
        {
            throw new OMVException(
                OMVErrorMsg::E_EXEC_FAILED,
                $cmd,
                implode("\n", $output)
            );
        }
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

        // Check if shared folder is set to none
        if (is_null($object['sharedfolderref']))
        {
            throw new OMVException(
                OMVErrorMsg::E_MISC_FAILURE,
                gettext("Please select a shared folder.")
            );
        }

        // Check to see if shared folder is used by certain plugins
        $pluginCheck = array();

        $pluginCheck[] = array(
            "xpath"        => "//services/calibre",
            "sharedfolder" => "data.sharedfolderref",
            "msg"          => "Calibre"
        );

        $pluginCheck[] = array(
            "xpath"        => "//services/developer",
            "sharedfolder" => "sharedfolderref",
            "msg"          => "Developer"
        );

        $pluginCheck[] = array(
            "xpath"        => "//services/mysql",
            "sharedfolder" => "data.sharedfolderref",
            "msg"          => "MySQL"
        );

        $pluginCheck[] = array(
            "xpath"        => "//services/virtualbox",
            "sharedfolder" => "machines.sharedfolderref",
            "msg"          => "Virtualbox"
        );


        foreach($pluginCheck as $plugin)
        {
            $objectCheck = $xmlConfig->get($plugin['xpath']);
            if ( !is_null($objectCheck))
            {
                $xPath = sprintf("%s[%s='%s']", $plugin['xpath'], $plugin['sharedfolder'], $object['sharedfolderref']);
                if (TRUE === $xmlConfig->exists($xPath))
                {
                    throw new OMVException(
                        OMVErrorMsg::E_MISC_FAILURE,
                        gettext(sprintf("Permissions for the %s data directory cannot be reset.", $plugin['msg']))
                    );
                }
            }
        }

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
