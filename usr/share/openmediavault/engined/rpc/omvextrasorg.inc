<?php
/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2017 OpenMediaVault Plugin Developers
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
require_once('openmediavault/globals.inc');
require_once('openmediavault/functions.inc');

class OMVRpcServiceOmvExtras extends \OMV\Rpc\ServiceAbstract
{
    public function getName()
    {
        return 'omvextras';
    }

    public function initialize()
    {
        $this->registerMethod('getRepoList');
        $this->registerMethod('getRepo');
        $this->registerMethod('setRepo');
        $this->registerMethod('deleteRepo');
        $this->registerMethod('doUpdate');

        $this->registerMethod('getArch');

        $this->registerMethod('getKernel');
        $this->registerMethod('setKernel');
        $this->registerMethod('getKernelList');
        $this->registerMethod('setBootKernel');

        $this->registerMethod('doCommand');
    }

    public function getRepoList($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.common.getlist');

        // Get the list of configuration objects.
        $db = \OMV\Config\Database::getInstance();
        $objects = $db->getAssoc('conf.system.omvextras');

        // Filter the result objects.
        return $this->applyFilter($objects, $params['start'], $params['limit'],
            $params['sortfield'], $params['sortdir']);
    }

    public function getRepo($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.common.objectuuid');

        // Get the configuration object.
        $db = \OMV\Config\Database::getInstance();

        return $db->getAssoc('conf.system.omvextras', $params['uuid']);
    }

    public function setRepo($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.omvextras.setrepo');

        // Prepare the configuration object.
        $object = new \OMV\Config\ConfigObject('conf.system.omvextras');
        $object->setAssoc($params);

        // Set the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $db->set($object);

        // Set apt module to dirty
        $dispatcher = \OMV\Engine\Notify\Dispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY,
            'org.openmediavault.conf.system.apt.distribution');

        // Apply the changes immediatelly (no user interaction is wanted).
        \OMV\Rpc\Rpc::call("Config", "applyChanges", [
            "modules" => [ "apt" ],
            "force" => TRUE
        ], $context);

        \OMV\Rpc\Rpc::call('Apt', 'update', NULL, $context);

          // Return the configuration object.
        return $object->getAssoc();
    }

    public function deleteRepo($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.common.objectuuid');

        // Delete the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.system.omvextras', $params['uuid']);
        $db->delete($object);

        // Return the deleted configuration object.
        return $object->getAssoc();
    }

    public function doUpdate($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Check if the module is marked as dirty. Abort this RPC
        // immediatelly in this case, otherwise the added/removed
        // repositories will not be included/excluded.
        if ($this->isModuleDirty('apt'))
            throw new \OMV\Config\ConfigDirtyException();
        return \OMV\Rpc\Rpc::call('Apt', 'update', NULL, $context);
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
        $grub = '/boot/grub/grub.cfg';
        if (file_exists($grub)) {
            $cmd = new \OMV\System\Process(sprintf('grep ^menuentry %s', $grub));
            $cmd->execute($output);

            foreach ($output as $kernelk => $kernelv) {
                $line = str_replace('"', '\'', $kernelv);
                $name = explode('\'', $line);

                if (empty($name[1]))
                    continue;

                $kernels[] = [
                    'key'  => $kernelk,
                    'name' => $name[1]
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
                if ($params['command'] == 'aptclean') {
                    $cmdBin = 'omv-aptclean';
                } else {
                    $cmdBin = 'omv-grubiso';
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
