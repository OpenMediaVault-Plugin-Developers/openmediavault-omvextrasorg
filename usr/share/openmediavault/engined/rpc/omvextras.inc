<?php
/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2021 OpenMediaVault Plugin Developers
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

class OMVRpcServiceOmvExtras extends \OMV\Rpc\ServiceAbstract
{
    public function getName()
    {
        return 'omvextras';
    }

    public function initialize()
    {
        $this->registerMethod('get');
        $this->registerMethod('set');

        $this->registerMethod('getDocker');
        $this->registerMethod('setDocker');
        $this->registerMethod('getPortainer');
        $this->registerMethod('setPortainer');
        $this->registerMethod('getYacht');
        $this->registerMethod('setYacht');
        $this->registerMethod('getUrl');

        $this->registerMethod('clean');
        $this->registerMethod('doCommand');
    }

    public function get($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Get the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.system.omvextras');
        // backports status
        $backports =  file_exists('/etc/apt/sources.list.d/openmediavault-kernel-backports.list');
        $object->add('backportsStatus', 'boolean', $backports);

        return $object->getAssoc();
    }

    public function set($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.omvextras.set');
        // Get the existing configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.system.omvextras');
        $object->set('testing',$params['testing']);
        $db->set($object);
        // Set omvextras module to dirty
        $dispatcher = \OMV\Engine\Notify\Dispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY,
            'org.openmediavault.conf.system.omvextras');
        // Execute command
        $cmd = new \OMV\System\Process('omv-changebackports', $params['backportsStatus'] ? 'YES' : 'NO');
        $cmd->execute($output);
        // Return the configuration object.
        return $object->getAssoc();
    }

    public function getDocker($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Get the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.system.omvextras');
        // docker status
        $cmdLine = "dpkg-query -W -f='\${Version}' docker-ce docker.io 2>/dev/null";
        $cmd = new \OMV\System\Process($cmdLine);
        $cmd->setQuiet(true);
        $output = [];
        $cmd->execute($output, $exitStatus);
        if (!empty($output)) {
            // Get docker service status
            $systemCtl = new \OMV\System\SystemCtl('docker');
            if ($systemCtl->isActive()) {
                $running = gettext("and running");
            } else {
                $running = gettext("and not running");
            }
            $status = sprintf('%s %s', gettext("Installed"), $running);
            $version = $output[0];
        } else {
            $status = gettext("Not installed");
            $version = gettext("n/a");
        }
        $object->add('dockerStatus', 'string', $status);
        $object->add('dockerVersion', 'string', $version);
        return $object->getAssoc();
    }

    public function setDocker($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.omvextras.setdocker');
        // Get the existing configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.system.omvextras');
        // Check for illegal path
        if ( preg_match('#^/sharedfolders/#', $params['dockerStorage']) === 1 ) {
            throw new \OMV\Exception(gettext("Illegal docker storage path - cannot use /sharedfolders paths"));
        }
        $object->set('dockerStorage',$params['dockerStorage']);
        $db->set($object);
        // Apply the changes immediatelly (no user interaction is wanted).
        \OMV\Rpc\Rpc::call('Config', 'applyChanges', [
            'modules' => [ 'omvextras' ],
            'force' => TRUE
        ], $context);
        // Rewrite daemon.json and restart if necessary
        $cmdLine = 'omv-installdocker docker config';
        $cmd = new \OMV\System\Process($cmdLine);
        $cmd->setQuiet(true);
        $output = [];
        $cmd->execute($output, $exitStatus);
        // Return the configuration object.
        return $object->getAssoc();
    }


    public function getPortainer($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Get the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.system.omvextras');

        // portainer status
        $cmdLine = "docker container ls --all --format='{{.Names}}|{{.Status}}' | awk -F \"|\" 'tolower(\$1) == \"portainer\" { print \$2 }'";
        $cmd = new \OMV\System\Process($cmdLine);
        $cmd->setQuiet(true);
        $output = [];
        $cmd->execute($output, $exitStatus);
        if (empty($output[0])) {
            $status = 'No portainer container found.';
            $status2 = false;
            $url = sprintf("<a href='https://%s:%d/' target='_blank'>Portainer</a>", gethostname(), $object->get('webport'));
        } else {
            $status = $output[0];
            $status2 = true;
            $url = '';
        }
        $object->add('portainerStatus', 'string', $status);
        $object->add('portainerStatus2', 'boolean', $status2);
        $object->add('url', 'string', $url);
        return $object->getAssoc();
    }

    public function setPortainer($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.omvextras.setportainer');
        // Get the existing configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.system.omvextras');
        // Check for same web and agent port
        if ( $params['webport'] == $params['agentport'] ) {
            throw new \OMV\Exception(gettext("Web port and agent port cannot be the same."));
        }
        $object->set('agentport',$params['agentport']);
        $object->set('webport',$params['webport']);
        $db->set($object);
        // Return the configuration object.
        return $object->getAssoc();
    }


    public function getYacht($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Get the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.system.omvextras');
        // yacht status
        $cmdLine = "docker container ls --all --format='{{.Names}}|{{.Status}}' | awk -F \"|\" 'tolower(\$1) == \"yacht\" { print \$2 }'";
        $cmd = new \OMV\System\Process($cmdLine);
        $cmd->setQuiet(true);
        $output = [];
        $cmd->execute($output, $exitStatus);
        if (empty($output[0])) {
            $status = 'No yacht container found.';
            $status2 = false;
            $url = sprintf("<a href='https://%s:%d/' target='_blank'>Yacht</a>", gethostname(), $object->get('yachtport'));
        } else {
            $status = $output[0];
            $status2 = true;
            $url = '';
        }
        $object->add('yachtStatus', 'string', $status);
        $object->add('yachtStatus2', 'boolean', $status2);
        $object->add('url', 'string', $url);
        return $object->getAssoc();
    }

    public function setYacht($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.omvextras.setyacht');
        // Get the existing configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.system.omvextras');
        $object->set('yachtport',$params['yachtport']);
        $db->set($object);
        // Return the configuration object.
        return $object->getAssoc();
    }

    public function getUrl($params, $context)
    {
    }

    public function clean($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Create the background process.
        return $this->execBgProc(function($bgStatusFilename, $bgOutputFilename)
            use ($params) {
                $cmd = new \OMV\System\Process("omv-aptclean");
                $cmd->setRedirect2to1();
                $cmdLine = $cmd->getCommandLine();
                if (0 !== $this->exec($cmdLine, $output, $bgOutputFilename))
                    throw new \OMV\ExecException($cmdLine, $output);
                return $output;
            }
        );
    }

    public function doCommand($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.omvextras.docommand');
        // Create the background process.
        return $this->execBgProc(function($bgStatusFilename, $bgOutputFilename)
            use ($params) {
                $cmdParams = '';
                switch ($params['command']) {
                    case 'docker_restart':
                        $cmdBin = 'echo "Restarting docker ..." && systemctl restart docker.service';  break;
                    case 'docker_install':
                    case 'docker_remove':
                    case 'portainer_install':
                    case 'portainer_remove':
                    case 'yacht_install':
                    case 'yacht_remove':
                        $cmdBin = 'omv-installdocker';
                        $cmdParams = str_replace('_',' ',$params['command']);
                        break;
                    case 'iptables_legacy':
                        $cmdBin = 'update-alternatives --set iptables /usr/sbin/iptables-legacy';  break;
                    case 'iptables_nft':
                        $cmdBin = 'update-alternatives --set iptables /usr/sbin/iptables-nft';  break;
                }
                $cmd = new \OMV\System\Process($cmdBin, $cmdParams);
                $cmdLine = $cmd->getCommandLine();
                if (0 !== $this->exec($cmdLine, $output, $bgOutputFilename))
                    throw new \OMV\ExecException($cmdLine, $output);
                return $output;
            }
        );
    }
}
