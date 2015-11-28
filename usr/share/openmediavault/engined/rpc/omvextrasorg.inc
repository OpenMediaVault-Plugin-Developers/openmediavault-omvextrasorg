<?php
/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2015 OpenMediaVault Plugin Developers
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

    private static $commands = array("aptclean","installbackports","installheaders");

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
        return "/usr/sbin/omv-support.sh";
    }

    private function getShowkey()
    {
        return "/usr/sbin/omv-showkey";
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

        $this->registerMethod("doCommand");

        $this->registerMethod("getSupportInfo");
        $this->registerMethod("getProcessList");
        $this->registerMethod("doSend");

        $this->registerMethod("doResetPerms");
        $this->registerMethod("getSharedFolderInUseList");

        $this->registerMethod("getRootFolders");
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

        // Get architecture
        // arch values
        // 0 - armel
        // 1 - armv6l armhf
        // 2 - armv7l armhf
        // 3 - i386
        // 4 - amd64
        $arch = 0;

        $uname = php_uname();

        if ( strpos($uname, "armv6l") !== false  )
        {
            $arch = 1;
        }
        elseif ( strpos($uname, "armv7l") !== false )
        {
            $arch = 2;
        }
        elseif ( strpos($uname, "486") !== false || strpos($uname, "686") !== false )
        {
            $arch = 3;
        }
        elseif ( strpos($uname, "amd64") !== false  )
        {
            $arch = 4;
        }

        // Create repo array
        $result = array();

        $result[] = array(
            "id"          => "btsync",
            "type"        => gettext("Backup"),
            "enable"      => $object['btsync'],
            "repo"        => gettext("BTSync"),
            "description" => gettext("BTSync repository")
        );

        // must be amd64
        if ( $arch > 3 )
        {
            $result[] = array(
                "id"          => "gluster",
                "type"        => gettext("Filesystems"),
                "enable"      => $object['gluster'],
                "repo"        => gettext("GlusterFS"),
                "description" => gettext("Do not enable this repository unless you are using the GlusterFS filesystem (uncommon)")
            );
        }

        $result[] = array(
            "id"          => "greyhole",
            "type"        => gettext("Filesystems"),
            "enable"      => $object['greyhole'],
            "repo"        => gettext("Greyhole"),
            "description" => gettext("Greyhole repository")
        );

        // no arm arches
        if ( $arch > 2 )
        {
            $result[] = array(
                "id"          => "hwraid",
                "type"        => gettext("Utilities"),
                "enable"      => $object['hwraid'],
                "repo"        => gettext("HWRaid"),
                "description" => gettext("Modules for different hardware raid controllers")
            );
        }

        $result[] = array(
            "id"          => "miller",
            "type"        => gettext("Downloaders"),
            "enable"      => $object['miller'],
            "repo"        => gettext("Download Managers"),
            "description" => gettext("Download managers for newsgroups, torrents, usenet, and streaming")
        );

        $result[] = array(
            "id"          => "millertesting",
            "type"        => gettext("Downloaders"),
            "enable"      => $object['millertesting'],
            "repo"        => gettext("Download Managers Testing"),
            "description" => gettext("Testing repository for download managers")
        );

        // no armel
        if ( $arch > 0 )
        {
            $result[] = array(
                "id"          => "mono",
                "type"        => gettext("Multimedia"),
                "enable"      => $object['mono'],
                "repo"        => gettext("Mono"),
                "description" => gettext("Mono and mono-project.com")
            );

            $result[] = array(
                "id"          => "monotesting",
                "type"        => gettext("Multimedia"),
                "enable"      => $object['monotesting'],
                "repo"        => gettext("Mono Testing"),
                "description" => gettext("Testing repository for Mono")
            );
        }

        // no arm arches
        if ( $arch > 2 )
        {
            $result[] = array(
                "id"          => "docker",
                "type"        => gettext("Virtualization"),
                "enable"      => $object['docker'],
                "repo"        => gettext("Docker"),
                "description" => gettext("Docker repository")
            );

            $result[] = array(
                "id"          => "dockertesting",
                "type"        => gettext("Virtualization"),
                "enable"      => $object['dockertesting'],
                "repo"        => gettext("Docker Testing"),
                "description" => gettext("Testing repository for Docker")
            );
        }

        $result[] = array(
            "id"          => "enable",
            "type"        => gettext("All"),
            "enable"      => $object['enable'],
            "repo"        => gettext("OMV-Extras.org"),
            "description" => gettext("Main package repository")
        );

        $result[] = array(
            "id"          => "testing",
            "type"        => gettext("All"),
            "enable"      => $object['testing'],
            "repo"        => gettext("OMV-Extras.org Testing"),
            "description" => gettext("Release candidates")
        );

        // no armel or armv6l
        if ( $arch > 1 )
        {
            $result[] = array(
                "id"          => "plex",
                "type"        => gettext("Multimedia"),
                "enable"      => $object['plex'],
                "repo"        => gettext("Plex"),
                "description" => gettext("Plex Media Server repository")
            );
        }

        // no arm arches
        if ( $arch > 2 )
        {
            $result[] = array(
                "id"          => "vbox",
                "type"        => gettext("Virtualization"),
                "enable"      => $object['vbox'],
                "repo"        => gettext("Virtualbox"),
                "description" => gettext("Virtualbox repository")
            );
        }

        // no armel
        if ( $arch > 0 )
        {
            $result[] = array(
                "id"          => "vdr",
                "type"        => gettext("Multimedia"),
                "enable"      => $object['vdr'],
                "repo"        => gettext("VDR"),
                "description" => gettext("e-tobi.net repository")
            );

            $result[] = array(
                "id"          => "vpn",
                "type"        => gettext("Remote"),
                "enable"      => $object['vpn'],
                "repo"        => gettext("VPN"),
                "description" => gettext("Plugins for setting up virtual private networks")
            );
        }

        // amd64 only
        if ( $arch > 3 )
        {
            $result[] = array(
                "id"          => "zfs",
                "type"        => gettext("Filesystems"),
                "enable"      => $object['zfs'],
                "repo"        => gettext("ZFS"),
                "description" => gettext("Do not enable this repository unless you are using the ZFS filesystem (uncommon)")
            );

            $result[] = array(
                "id"          => "zfstesting",
                "type"        => gettext("Filesystems"),
                "enable"      => $object['zfstesting'],
                "repo"        => gettext("ZFS Testing"),
                "description" => gettext("Do not enable this testing repository unless you are using the ZFS filesystem (uncommon)")
            );
        }

        if ( $object['developer'] == "0827" )
        {
            $result[] = array(
                "id"          => "beta",
                "type"        => gettext("All"),
                "enable"      => $object['beta'],
                "repo"        => gettext("Beta"),
                "description" => gettext("Beta repository")
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

        if ( boolval($repos['monotesting']) === true )
            $repos['mono'] = true;

        if ( boolval($repos['dockertesting']) === true )
            $repos['docker'] = true;

        if ( boolval($repos['zfstesting']) === true )
            $repos['zfs'] = true;

        $uname = php_uname();
        if ( strpos($uname, "amd64") === false )
        {
            $repos['zfs'] = false;
            $repos['zfstesting'] = false;
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
            "zfstesting"    => array_boolval($repos, 'zfstesting'),
            "gluster"       => array_boolval($repos, 'gluster'),
            "mono"          => array_boolval($repos, 'mono'),
            "monotesting"   => array_boolval($repos, 'monotesting'),
            "docker"        => array_boolval($repos, 'docker'),
            "dockertesting" => array_boolval($repos, 'dockertesting'),
            "hwraid"        => array_boolval($repos, 'hwraid'),
            "beta"          => array_boolval($repos, 'beta'),
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
                "key"     : { "type" : "string", "optional" : true },
                "comment" : { "type" : "string" }
            }
        }');

        // Prepare configuration data.
        $object = array(
            "uuid"    => ($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ? OMVUtil::uuid() : $params['uuid'],
            "name"    => $params['name'],
            "repo"    => $params['repo'],
            "key"     => $params['key'],
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
            "name" => gettext("Style #1")
        );

        $styles[] = array(
            "cmd"  => "style2",
            "name" => gettext("Style #2")
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
            "cmd"  => "degraded",
            "name" => gettext("Degraded Array Info")
        );

        $styles[] = array(
            "cmd"  => "iface",
            "name" => gettext("System - iface")
        );

        $styles[] = array(
            "cmd"  => "mntent",
            "name" => gettext("System - mntent")
        );

        $styles[] = array(
            "cmd"  => "sharedfolder",
            "name" => gettext("System - sharedfolder")
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
            "zfstesting"    => array_boolval($oldObject, 'zfstesting'),
            "gluster"       => array_boolval($oldObject, 'gluster'),
            "mono"          => array_boolval($oldObject, 'mono'),
            "monotesting"   => array_boolval($oldObject, 'monotesting'),
            "docker"        => array_boolval($oldObject, 'docker'),
            "dockertesting" => array_boolval($oldObject, 'dockertesting'),
            "hwraid"        => array_boolval($oldObject, 'hwraid'),
            "beta"          => array_boolval($oldObject, 'beta'),
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

    public function doCommand($params, $context)
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
                "command"  : { "type" : "string" }
            }
        }');

        if (!in_array($params['command'], self::$commands))
        {
            throw new OMVException(
                OMVErrorMsg::E_MISC_OPERATION_DENIED,
                sprinf("Unknown command - %s", $params['command'])
            );
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

            $cmd = sprintf("export LANG=C; omv-mkconf %s %s 2>&1", $this->getPluginName(), $params['command'] );
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
                    array("section" => "00", "module_name" => "debian-version"),
                    array("section" => "00", "module_name" => "omv-version"),
                    array("section" => "10", "module_name" => "date"),
                    array("section" => "10", "module_name" => "hostname"),
                    array("section" => "10", "module_name" => "locale"),
                    array("section" => "10", "module_name" => "uname"),
                    array("section" => "10", "module_name" => "uptime"),
                    array("section" => "20", "module_name" => "interfaces"),
                    array("section" => "20", "module_name" => "iptables"),
                    array("section" => "20", "module_name" => "netstat"),
                    array("section" => "20", "module_name" => "route"),
                    array("section" => "30", "module_name" => "mdadm"),
                    array("section" => "30", "module_name" => "monit"),
                    array("section" => "31", "module_name" => "hpraid"),
                    array("section" => "40", "module_name" => "blkid"),
                    array("section" => "40", "module_name" => "df"),
                    array("section" => "40", "module_name" => "fstab"),
                    array("section" => "40", "module_name" => "mountinfo"),
                    array("section" => "40", "module_name" => "partitions"),
                    array("section" => "50", "module_name" => "apt-sources"),
                    array("section" => "50", "module_name" => "dpkg"),
                    array("section" => "50", "module_name" => "omv-plugins"),
                    array("section" => "61", "module_name" => "lsmod")
                );

                foreach ($modules as $module)
                {
                    if ( boolval($object[$module["module_name"]]) === true )
                    {
                        $path = sprintf("export LANG=C; %s%s-%s", $this->getSysinfoModuleDir(), $module["section"], $module["module_name"]);
                        OMVUtil::exec($path, $result);
                    }
                }
                break;

            case "style2":
                $cmd = sprintf("export LANG=C; %s webgui", $this->getOmvSupport() );
                OMVUtil::exec($cmd, $result);
                break;

            case "degraded":
                $cmd = sprintf("export LANG=C; cat /proc/mdstat", $this->getOmvSupport() );
                OMVUtil::exec($cmd, $result);

                $cmd = sprintf("export LANG=C; blkid", $this->getOmvSupport() );
                OMVUtil::exec($cmd, $result);

                $cmd = sprintf("export LANG=C; fdisk -l", $this->getOmvSupport() );
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
        $cmd = 'export LANG=C; ps axww -o pid,user,%cpu,%mem,stat,args 2>&1';
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

            $cmd = sprintf("export LANG=C; omv-mkconf %s sendsupport 2>&1", $this->getPluginName() );
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

            $cmd = sprintf("export LANG=C; omv-mkconf %s resetperms 2>&1", $this->getPluginName() );
            $this->exec($cmd, $output, $bgOutputFilename);
            $this->finalizeBgProcStatus($bgStatusFilename, $output);

        } catch(Exception $e) {
            $this->finalizeBgProcStatus($bgStatusFilename, "", $e);
            exit(1);
        }
    }

    public function getSharedFolderInUseList($params, $context)
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

        // Get all configured shared folder configuration objects.
        $xPath = "//system/shares/sharedfolder";
        $objects = $xmlConfig->getList($xPath);
        if (is_null($objects)) {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $xPath
            );
        }

        // Add additional informations.
        foreach ($objects as $objectk => &$objectv) {
            $output = array();
            $cmd = sprintf("omv-showsharedfolder %s", $objectv['name']);
            OMVUtil::exec($cmd, $output, $result);
            $objectv['inuse'] = implode("<br />", $output);
        }

        // Filter the result.
        return $this->applyFilter($objects, $params['start'], $params['limit'],
            $params['sortfield'], $params['sortdir']);
    }

    public function getRootFolders($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // List the directory contents.
        $result = array();
        $path = sprintf("/%s", rtrim($params['path'], "/"));
        if ( strlen($path) == 0 )
            $path = "/";

        foreach (new DirectoryIterator($path) as $item)
        {
            if ($item->isDot())
                continue;
            if (!$item->isDir())
                continue;
            $result[] = $item->getFilename();
        }
        return $result;
    }
}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServiceOmvExtrasOrg());
