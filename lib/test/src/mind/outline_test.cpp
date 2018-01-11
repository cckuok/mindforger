/*
 outline_test.cpp     MindForger test

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "../../../src/gear/file_utils.h"
#include "../../../src/model/outline.h"
#include "../../../src/model/outline_type.h"
#include "../../../src/model/note.h"
#include "../../../src/model/tag.h"
#include "../../../src/model/stencil.h"
#include "../../../src/model/resource_types.h"
#include "../../../src/mind/mind.h"
#include "../../../src/install/installer.h"

using namespace std;

void dumpOutline(m8r::Outline*& outline);

TEST(OutlineTestCase, NewAndDeleteOutline) {
    string repositoryDir{"/tmp/mf-unit-repository"};
    m8r::removeDirectoryRecursively(repositoryDir.c_str());
    m8r::Installer installer{};
    installer.createEmptyMindForgerRepository(repositoryDir);
    string oFile{"/tmp/mf-unit-repository/memory/outline.md"};
    string oContent{"# Test Outline\n\nOutline text.\n\n## Note 1\nNote 1 text.\n"};
    m8r::stringToFile(oFile,oContent);

    m8r::Configuration configuration{repositoryDir};
    m8r::Mind mind{configuration};
    mind.think();

    EXPECT_EQ(mind.remind().getOutlinesCount(), 1);
    EXPECT_EQ(mind.remind().getNotesCount(), 1);

    // just delete 1 outline w/ 1 note from memory (and check w/ Valgrind)
    mind.outlineForget(mind.remind().getOutlines()[0]->getKey());

    EXPECT_EQ(mind.remind().getOutlinesCount(), 0);
    EXPECT_EQ(mind.remind().getNotesCount(), 0);
}

TEST(OutlineTestCase, NewOutlineFromStencil) {
    // prepare M8R repository and let the mind think...
    string repositoryDir{"/tmp/mf-unit-repository-o"};
    m8r::removeDirectoryRecursively(repositoryDir.c_str());
    m8r::Installer installer{};
    installer.createEmptyMindForgerRepository(repositoryDir);
    string stencilFile{"/tmp/mf-unit-repository-o/stencils/outlines/o-s.md"};
    string stencilContent{"# Stencil Test Outline\n\nOutline text.\n\n## Stencil Note 1\nNote 1 text.\n\n##Stencil Note 2\nNote 2 text.\n"};
    m8r::stringToFile(stencilFile,stencilContent);

    m8r::Configuration configuration{repositoryDir};
    m8r::Mind mind{configuration};
    m8r::Memory& memory = mind.remind();
    mind.think();

    // create Outline using a stencil from MEMORY
    vector<m8r::Stencil*>& stencils = memory.getStencils();
    cout << endl << "Loaded Outline STENCILS: " << stencils.size();
    // IMPROVE constructor call is WRONG > complete parameters
    string title{"MIND's stencil Outline"};
    mind.outlineNew(
                &title,
                mind.ontology().findOrCreateOutlineType(m8r::OutlineType::KeyOutline()),
                1,
                2,
                mind.ontology().findOrCreateTag(m8r::Tag::KeyCool()),
                stencils[0]);

    // create stencil MANUALLY (stencil file does NOT have to exist)
    unique_ptr<m8r::Stencil> outlineStencil{
        new m8r::Stencil{
            string{"/tmp/mf-unit-repository-o/stencils/outlines/grow.md"},
            string{"Manual stencil"}}};
    // IMPROVE constructor call is WRONG > complete parameters
    title.assign("MANUAL stencil Outline");
    mind.outlineNew(
                &title,
                mind.ontology().findOrCreateOutlineType(m8r::OutlineType::KeyGrow()),
                3,
                5,
                mind.ontology().findOrCreateTag(m8r::Tag::KeyImportant()),
                outlineStencil.get());

    // asserts
    EXPECT_EQ(mind.remind().getOutlinesCount(), 2);
    EXPECT_EQ(mind.remind().getNotesCount(), 3);
}