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
require_once("openmediavault/module.inc");
require_once("openmediavault/error.inc");

class OMVModuleOmvExtrasOrg extends OMVModuleServiceAbstract
    implements OMVINotifyListener {

    public function getName() {
        return "omvextrasorg";
    }

    private function getPluginName() {
        return strtolower( $this->getName() );
    }

    private function getEventMessagePath() {
        $path = sprintf( "org.openmediavault.system.%s", $this->getPluginName() );
        return $path;
    }

    private function getEventMessagePathRepo() {
        $path = sprintf( "%s.repos.repo", $this->getEventMessagePath() );
        return $path;
    }

    public function applyConfig() {
        global $xmlConfig;

        $cmd = sprintf("omv-mkconf %s 2>&1", $this->getPluginName() );
        if(0 !== $this->exec($cmd, $output))
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED, $cmd, implode("\n", $output));
    }

    function bindListeners(OMVNotifyDispatcher $dispatcher) {
        $dispatcher->addListener(
            OMV_NOTIFY_MODIFY,
            $this->getEventMessagePath(),
            array($this, "setDirty"));

        $dispatcher->addListener(
            OMV_NOTIFY_CREATE | OMV_NOTIFY_MODIFY | OMV_NOTIFY_DELETE,
            $this->getEventMessagePathRepo(),
            array($this, "setDirty"));
    }
}

// Register module.
$moduleMgr = &OMVModuleMgr::getInstance();
$moduleMgr->registerModule(new OMVModuleOmvExtrasOrg());
?>
