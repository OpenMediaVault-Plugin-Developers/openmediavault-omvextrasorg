<?php
/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2020 OpenMediaVault Plugin Developers
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
        $this->registerMethod('getSettings');
        $this->registerMethod('setSettings');
        $this->registerMethod('getDocker');
        $this->registerMethod('setDocker');
        $this->registerMethod('getCockpit');

        $this->registerMethod('getArch');

        $this->registerMethod('getKernel');
        $this->registerMethod('setKernel');
        $this->registerMethod('getKernelList');
        $this->registerMethod('setBootKernel');

        $this->registerMethod('doCommand');
        $this->registerMethod('doHold');
        $this->registerMethod('doProxmox');
    }

    public function getSettings($params, $context)
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

    public function setSettings($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.omvextras.setsettings');
        // Get the existing configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.system.omvextras');
        $object->set('testing',$params['testing']);
        $object->set('extras',$params['extras']);
        $db->set($object);
        // Set omvextras module to dirty
        $dispatcher = \OMV\Engine\Notify\Dispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY,
            'org.openmediavault.conf.system.omvextras');
        // Apply the changes immediatelly (no user interaction is wanted).
        \OMV\Rpc\Rpc::call('Config', 'applyChanges', [
            'modules' => [ 'omvextras' ],
            'force' => TRUE
        ], $context);
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
        $cmdLine = "dpkg-query -W -f='\${Version}' docker-ce";
        $cmd = new \OMV\System\Process($cmdLine);
        $cmd->setQuiet(true);
        $output = [];
        $cmd->execute($output, $exitStatus);
        if ((0 == $exitStatus) && (!empty($output))) {
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

        // portainer status
        $cmdLine = "docker container ls --all --format='{{.Names}}|{{.Status}}' | awk -F \"|\" 'tolower(\$1) == \"portainer\" { print \$2 }'";
        $cmd = new \OMV\System\Process($cmdLine);
        $cmd->setQuiet(true);
        $output = [];
        $cmd->execute($output, $exitStatus);
        if (empty($output[0])) {
            $status = 'No portainer container found.';
            $status2 = false;
        } else {
            $status = $output[0];
            $status2 = true;
        }
        $object->add('portainerStatus', 'string', $status);
        $object->add('portainerStatus2', 'boolean', $status2);

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
        // Check for same web and agent port
        if ( $params['webport'] == $params['agentport'] ) {
            throw new \OMV\Exception(gettext("Web port and agent port cannot be the same."));
        }
        $object->set('agentport',$params['agentport']);
        $object->set('dockerStorage',$params['dockerStorage']);
        $object->set('optout',$params['optout']);
        $object->set('webport',$params['webport']);
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

    public function getCockpit($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Get the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.system.omvextras');

        // cockpit status
        $cmdLine = "dpkg-query -W -f='\${Version}' cockpit";
        $cmd = new \OMV\System\Process($cmdLine);
        $cmd->setQuiet(true);
        $output = [];
        $cmd->execute($output, $exitStatus);
        if ((0 == $exitStatus) && (!empty($output))) {
            // Get docker service status
            $systemCtl = new \OMV\System\SystemCtl('cockpit.socket');
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
        $object->add('cockpitStatus', 'string', $status);
        $object->add('cockpitVersion', 'string', $version);

        return $object->getAssoc();
    }

    public function getArch($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Get architecture
        $object = [];
        $cmd = new \OMV\System\Process('dpkg --print-architecture');
        $cmd->execute($output);
        $object['arch'] = strtolower($output[0]);
        // Return object
        return $object;
    }

    public function getKernel($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Get boot kernel
        $grub = '/etc/default/grub';
        if (file_exists($grub)) {
            $line_grub = 'GRUB_DEFAULT=';
            $cmd = new \OMV\System\Process(sprintf('grep %s %s', $line_grub, $grub));
            $cmd->execute($output);
            $object['kernels'] = intval(str_replace($line_grub, '', $output[0]));
        }
        // Return object
        return $object;
    }

    public function setKernel($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
    }

    public function getKernelList($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.common.getlist');
        // Get configuration data.
        $kernels = [];
        $awkArgs = '-F "\'" \'$1 == "menuentry " { print $2 }\'';
        $grub = '/boot/grub/grub.cfg';
        if (file_exists($grub)) {
            $cmd = new \OMV\System\Process('awk', $awkArgs, $grub);
            $cmd->execute($output);
            foreach ($output as $kernelk => $kernelv) {
                if (empty($kernelv))
                    continue;
                $kernels[] = [
                    'key'  => $kernelk,
                    'name' => $kernelv
                ];
            }
        }
        return $this->applyFilter($kernels, $params['start'], $params['limit'],
            $params['sortfield'], $params['sortdir']
        );
    }

    public function setBootKernel($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Set boot kernel
        $grub = '/etc/default/grub';
        if (file_exists($grub)) {
            $cmdArgs = sprintf('-p -i -e "s/GRUB_DEFAULT=.*/GRUB_DEFAULT=%s/g" %s', $params['key'], $grub);
            $cmd = new \OMV\System\Process('perl', $cmdArgs);
            $cmd->execute($output);
            $cmd = new \OMV\System\Process('grub-set-default', $params['key']);
            $cmd->execute($output);
            $cmd = new \OMV\System\Process('update-grub');
            $cmd->execute($output);
        }
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
                    case 'update':
                        $cmdBin = 'apt-get update';  break;
                    case 'omv-update':
                        $cmdBin = 'omv-update';  break;
                    case 'upgrade':
                        $cmdBin = 'apt-get --yes upgrade';  break;
                    case 'dist-upgrade':
                        $cmdBin = 'apt-get --yes dist-upgrade';  break;
                    case 'clean':
                        $cmdBin = 'omv-aptclean';  break;
                    case 'docker_restart':
                        $cmdBin = 'echo "Restarting docker ..." && systemctl restart docker.service';  break;
                    case 'cockpit_install':
                    case 'cockpit_remove':
                    case 'docker_install':
                    case 'docker_remove':
                    case 'portainer_install':
                    case 'portainer_remove':
                        $cmdBin = 'omv-installdocker';
                        $cmdParams = str_replace('_',' ',$params['command']);
                        break;
                    default:
                        $cmdBin = 'omv-grubiso';
                        $cmdParams = $params['command'];
                }
                $cmd = new \OMV\System\Process($cmdBin, $cmdParams);
                $cmdLine = $cmd->getCommandLine();
                if (0 !== $this->exec($cmdLine, $output, $bgOutputFilename))
                    throw new \OMV\ExecException($cmdLine, $output);
                return $output;
            }
        );
    }

    public function doHold($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.omvextras.dohold');
        // Execute command
        $arch = $this->getArch([], $context);
        $packages = sprintf('linux-image-%s linux-headers-%s', $arch['arch'], $arch['arch']);
        $cmd = new \OMV\System\Process('apt-mark', $params['command'], $packages);
        $cmd->execute($output);
    }

    public function doProxmox($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.omvextras.doproxmox');
        // Create the background process.
        return $this->execBgProc(function($bgStatusFilename, $bgOutputFilename)
            use ($params) {
                if ($params['command'] == 'remove') {
                    $cmdBin = 'omv-removekernels';
                } else {
                    $cmdBin = 'omv-installproxmox';
                }
                $cmd = new \OMV\System\Process($cmdBin, $params['command']);
                $cmdLine = $cmd->getCommandLine();
                if (0 !== $this->exec($cmdLine, $output, $bgOutputFilename))
                    throw new \OMV\ExecException($cmdLine, $output);
                return $output;
            }
        );
    }
}
