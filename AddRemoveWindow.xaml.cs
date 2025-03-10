﻿using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows;

namespace RetroSpy
{
    public partial class AddRemoveWindow : Window
    {
        private readonly List<string> _excludedList;
        readonly IReadOnlyList<InputSource> _allSources;
        readonly Collection<string> _originalExcludedList;

        public AddRemoveWindow(IReadOnlyList<InputSource> allSources, Collection<string> excludedList)
        {
            _originalExcludedList = excludedList;
            _excludedList = new List<string>(excludedList);
            _allSources = allSources;

            InitializeComponent();

            PopulateListBoxes();
        }

        private void PopulateListBoxes()
        {
            IncludedListBox.Items.Clear();
            ExcludedListBox.Items.Clear();
            foreach (var source in _allSources)
            {
                if (_excludedList.Contains(source.Name))
                {
                    ExcludedListBox.Items.Add(source.Name);
                }
                else
                {
                    IncludedListBox.Items.Add(source.Name);
                }
            }
        }

        private void SaveButton_Click(object sender, RoutedEventArgs e)
        {
            _originalExcludedList.Clear();
            foreach (var source in _excludedList)
            {
                _originalExcludedList.Add(source);
            }
            Close();
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void Remove_Click(object sender, RoutedEventArgs e)
        {
            if (IncludedListBox.SelectedIndex != -1)
            {
                var selectedIndex = IncludedListBox.SelectedIndex;

                if (IncludedListBox.SelectedIndex == IncludedListBox.Items.Count - 1 && IncludedListBox.Items.Count > 1)
                    selectedIndex -= 1;
                else if (IncludedListBox.Items.Count == 1)
                    selectedIndex = -1;

                _excludedList.Add((string)IncludedListBox.Items[IncludedListBox.SelectedIndex]);

                PopulateListBoxes();
                IncludedListBox.SelectedIndex = selectedIndex;
            }
        }

        private void Add_Click(object sender, RoutedEventArgs e)
        {
            if (ExcludedListBox.SelectedIndex != -1)
            {
                var selectedIndex = ExcludedListBox.SelectedIndex;

                if (ExcludedListBox.SelectedIndex == ExcludedListBox.Items.Count - 1 && ExcludedListBox.Items.Count > 1)
                    selectedIndex -= 1;
                else if (ExcludedListBox.Items.Count == 1)
                    selectedIndex = -1;

                _excludedList.Remove((string)ExcludedListBox.Items[ExcludedListBox.SelectedIndex]);

                PopulateListBoxes();
                ExcludedListBox.SelectedIndex = selectedIndex;
            }
        }
    }
}

