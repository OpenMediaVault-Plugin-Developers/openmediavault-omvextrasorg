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
require_once("openmediavault/module.inc");
require_once("openmediavault/error.inc");

class OMVModuleOmvExtrasOrg extends OMVModuleServiceAbstract
    implements OMVINotifyListener {

    /**
     * Get the module name.
     */
    public function getName() {
        return "omvextrasorg";
    }

    /**
     * Generate the configuration.
     * @throw E_EXEC_FAILED
     */
    public function applyConfig() {
        global $xmlConfig;

        $cmd = "omv-mkconf omvextrasorg 2>&1";
        if(0 !== $this->exec($cmd, $output)) {
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED, $cmd, implode("\n", $output));
        }
    }

    /**
     * Bind listeners.
     */
    function bindListeners(OMVNotifyDispatcher $dispatcher) {
        $dispatcher->addListener(
            OMV_NOTIFY_MODIFY,
            "org.openmediavault.system.omvextrasorg",
            array($this, "setDirty"));
    }
}

// Register module.
$moduleMgr = &OMVModuleMgr::getInstance();
$moduleMgr->registerModule(new OMVModuleOmvExtrasOrg());

?>
