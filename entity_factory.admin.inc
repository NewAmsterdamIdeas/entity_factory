<?php

/**
 * Form constructor for the 'Manage fields' form of a bundle.
 *
 * Allows fields and pseudo-fields to be re-ordered.
 *
 * @see field_ui_field_overview_form_validate()
 * @see field_ui_field_overview_form_submit()
 * @ingroup forms
 */
function entity_factory_field_overview_form($form, &$form_state, $entity_type, $bundle, $schema=NULL) {
  require_once DRUPAL_ROOT . '/modules/field_ui/field_ui.admin.inc';
  $bundle = field_extract_bundle($entity_type, $bundle);
  field_ui_inactive_message($entity_type, $bundle);
  $admin_path = _field_ui_bundle_admin_path($entity_type, $bundle);

  // When displaying the form, make sure the list of fields is up-to-date.
  if (empty($form_state['post'])) {
    field_info_cache_clear();
  }

  // Gather bundle information.
  $instances = field_info_instances($entity_type, $bundle);
  $field_types = field_info_field_types();
  $widget_types = field_info_widget_types();

  $extra_fields = field_info_extra_fields($entity_type, $bundle, 'form');

  $form += array(
    '#entity_type' => $entity_type,
    '#bundle' => $bundle,
    '#fields' => array_keys($instances),
    '#extra' => array_keys($extra_fields),
  );


  $table = array(
    '#type' => 'field_ui_table',
    '#tree' => TRUE,
    '#header' => array(
      t('Add?'),
      t('Label'),
      t('Weight'),
      t('Parent'),
      t('Machine name'),
      t('Field type'),
      t('Widget'),
      array('data' => t('Operations/Help text'), 'colspan' => 2),
    ),
    '#parent_options' => array(),
    '#regions' => array(
      'main' => array('message' => t('No fields are present yet.')),
      'add_new' => array('title' => '&nbsp;'),
      'add_schema' => array('title' => t('Fields based on the @schema schema', array('@schema' => $schema['name']))),
    ),
    '#attributes' => array(
      'class' => array('field-ui-overview'),
      'id' => 'field-overview',
    ),
  );

  // Fields.
  foreach ($instances as $name => $instance) {
    $field = field_info_field($instance['field_name']);
    $admin_field_path = $admin_path . '/fields/' . $instance['field_name'];
    $table[$name] = array(
      '#attributes' => array('class' => array('draggable', 'tabledrag-leaf')),
      '#row_type' => 'field',
      '#region_callback' => 'field_ui_field_overview_row_region',
      'add' => array('#markup' => ''),
      'label' => array(
        '#markup' => check_plain($instance['label']),
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#title' => t('Weight for @title', array('@title' => $instance['label'])),
        '#title_display' => 'invisible',
        '#default_value' => $instance['widget']['weight'],
        '#size' => 3,
        '#attributes' => array('class' => array('field-weight')),
       ),
      'parent_wrapper' => array(
        'parent' => array(
          '#type' => 'select',
          '#title' => t('Parent for @title', array('@title' => $instance['label'])),
          '#title_display' => 'invisible',
          '#options' => $table['#parent_options'],
          '#empty_value' => '',
          '#attributes' => array('class' => array('field-parent')),
          '#parents' => array('fields', $name, 'parent'),
        ),
        'hidden_name' => array(
          '#type' => 'hidden',
          '#default_value' => $name,
          '#attributes' => array('class' => array('field-name')),
        ),
      ),
      'field_name' => array(
        '#markup' => $instance['field_name'],
      ),
      'type' => array(
        '#type' => 'link',
        '#title' => t($field_types[$field['type']]['label']),
        '#href' => $admin_field_path . '/field-settings',
        '#options' => array('attributes' => array('title' => t('Edit field settings.'))),
      ),
      'widget_type' => array(
        '#type' => 'link',
        '#title' => t($widget_types[$instance['widget']['type']]['label']),
        '#href' => $admin_field_path . '/widget-type',
        '#options' => array('attributes' => array('title' => t('Change widget type.'))),
      ),
      'edit' => array(
        '#type' => 'link',
        '#title' => t('edit'),
        '#href' => $admin_field_path,
        '#options' => array('attributes' => array('title' => t('Edit instance settings.'))),
      ),
      'delete' => array(
        '#type' => 'link',
        '#title' => t('delete'),
        '#href' => $admin_field_path . '/delete',
        '#options' => array('attributes' => array('title' => t('Delete instance.'))),
      ),
    );

    if (!empty($instance['locked'])) {
      $table[$name]['edit'] = array('#value' => t('Locked'));
      $table[$name]['delete'] = array();
      $table[$name]['#attributes']['class'][] = 'menu-disabled';
    }
  }

  // Non-field elements.
  foreach ($extra_fields as $name => $extra_field) {
    $table[$name] = array(
      '#attributes' => array('class' => array('draggable', 'tabledrag-leaf')),
      '#row_type' => 'extra_field',
      '#region_callback' => 'field_ui_field_overview_row_region',
      'add' => array('#markup' => ''),
      'label' => array(
        '#markup' => check_plain($extra_field['label']),
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#default_value' => $extra_field['weight'],
        '#size' => 3,
        '#attributes' => array('class' => array('field-weight')),
        '#title_display' => 'invisible',
        '#title' => t('Weight for @title', array('@title' => $extra_field['label'])),
      ),
      'parent_wrapper' => array(
        'parent' => array(
          '#type' => 'select',
          '#title' => t('Parent for @title', array('@title' => $extra_field['label'])),
          '#title_display' => 'invisible',
          '#options' => $table['#parent_options'],
          '#empty_value' => '',
          '#attributes' => array('class' => array('field-parent')),
          '#parents' => array('fields', $name, 'parent'),
        ),
        'hidden_name' => array(
          '#type' => 'hidden',
          '#default_value' => $name,
          '#attributes' => array('class' => array('field-name')),
        ),
      ),
      'field_name' => array(
        '#markup' => $name,
      ),
      'type' => array(
        '#markup' => isset($extra_field['description']) ? $extra_field['description'] : '',
        '#cell_attributes' => array('colspan' => 2),
      ),
      'edit' => array(
        '#markup' => isset($extra_field['edit']) ? $extra_field['edit'] : '',
      ),
      'delete' => array(
        '#markup' => isset($extra_field['delete']) ? $extra_field['delete'] : '',
      ),
    );
  }

  // Additional row: add new field.
  $max_weight = field_info_max_weight($entity_type, $bundle, 'form');
  $field_type_options = field_ui_field_type_options();
  $widget_type_options = field_ui_widget_type_options(NULL, TRUE);
  if ($field_type_options && $widget_type_options) {
    $name = '_add_new_field';
    $table[$name] = array(
      '#attributes' => array('class' => array('draggable', 'tabledrag-leaf', 'add-new')),
      '#row_type' => 'add_new_field',
      '#region_callback' => 'field_ui_field_overview_row_region',
      'add' => array('#markup' => ''),
      'label' => array(
        '#type' => 'textfield',
        '#title' => t('New field label'),
        '#title_display' => 'invisible',
        '#size' => 15,
        '#description' => t('Label'),
        '#prefix' => '<div class="label-input"><div class="add-new-placeholder">' . t('Add new field') .'</div>',
        '#suffix' => '</div>',
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#default_value' => $max_weight + 1,
        '#size' => 3,
        '#title_display' => 'invisible',
        '#title' => t('Weight for new field'),
        '#attributes' => array('class' => array('field-weight')),
        '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
      ),
      'parent_wrapper' => array(
        'parent' => array(
          '#type' => 'select',
          '#title' => t('Parent for new field'),
          '#title_display' => 'invisible',
          '#options' => $table['#parent_options'],
          '#empty_value' => '',
          '#attributes' => array('class' => array('field-parent')),
          '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
          '#parents' => array('fields', $name, 'parent'),
        ),
        'hidden_name' => array(
          '#type' => 'hidden',
          '#default_value' => $name,
          '#attributes' => array('class' => array('field-name')),
        ),
      ),
      'field_name' => array(
        '#type' => 'machine_name',
        '#title' => t('New field name'),
        '#title_display' => 'invisible',
        // This field should stay LTR even for RTL languages.
        '#field_prefix' => '<span dir="ltr">field_',
        '#field_suffix' => '</span>&lrm;',
        '#size' => 15,
        '#description' => t('A unique machine-readable name containing letters, numbers, and underscores.'),
        // 32 characters minus the 'field_' prefix.
        '#maxlength' => 26,
        '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
        '#machine_name' => array(
          'source' => array('fields', $name, 'label'),
          'exists' => '_field_ui_field_name_exists',
          'standalone' => TRUE,
          'label' => '',
        ),
        '#required' => FALSE,
      ),
      'type' => array(
        '#type' => 'select',
        '#title' => t('Type of new field'),
        '#title_display' => 'invisible',
        '#options' => $field_type_options,
        '#empty_option' => t('- Select a field type -'),
        '#description' => t('Type of data to store.'),
        '#attributes' => array('class' => array('field-type-select')),
        '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
      ),
      'widget_type' => array(
        '#type' => 'select',
        '#title' => t('Widget for new field'),
        '#title_display' => 'invisible',
        '#options' => $widget_type_options,
        '#empty_option' => t('- Select a widget -'),
        '#description' => t('Form element to edit the data.'),
        '#attributes' => array('class' => array('widget-type-select')),
        '#cell_attributes' => array('colspan' => 3),
        '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
      ),
      // Place the 'translatable' property as an explicit value so that contrib
      // modules can form_alter() the value for newly created fields.
      'translatable' => array(
        '#type' => 'value',
        '#value' => FALSE,
      ),
    );
  }

  // Additional row: add existing field.
  $existing_fields = field_ui_existing_field_options($entity_type, $bundle);
  if ($existing_fields && $widget_type_options) {
    // Build list of options.
    $existing_field_options = array();
    foreach ($existing_fields as $field_name => $info) {
      $text = t('@type: @field (@label)', array(
        '@type' => $info['type_label'],
        '@label' => $info['label'],
        '@field' => $info['field'],
      ));
      $existing_field_options[$field_name] = truncate_utf8($text, 80, FALSE, TRUE);
    }
    asort($existing_field_options);
    $name = '_add_existing_field';
    $table[$name] = array(
      '#attributes' => array('class' => array('draggable', 'tabledrag-leaf', 'add-new')),
      '#row_type' => 'add_new_field',
      '#region_callback' => 'field_ui_field_overview_row_region',
      'add' => array('#markup' => ''),
      'label' => array(
        '#type' => 'textfield',
        '#title' => t('Existing field label'),
        '#title_display' => 'invisible',
        '#size' => 15,
        '#description' => t('Label'),
        '#attributes' => array('class' => array('label-textfield')),
        '#prefix' => '<div class="label-input"><div class="add-new-placeholder">' . t('Add existing field') .'</div>',
        '#suffix' => '</div>',
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#default_value' => $max_weight + 2,
        '#size' => 3,
        '#title_display' => 'invisible',
        '#title' => t('Weight for added field'),
        '#attributes' => array('class' => array('field-weight')),
        '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
      ),
      'parent_wrapper' => array(
        'parent' => array(
          '#type' => 'select',
          '#title' => t('Parent for existing field'),
          '#title_display' => 'invisible',
          '#options' => $table['#parent_options'],
          '#empty_value' => '',
          '#attributes' => array('class' => array('field-parent')),
          '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
          '#parents' => array('fields', $name, 'parent'),
        ),
        'hidden_name' => array(
          '#type' => 'hidden',
          '#default_value' => $name,
          '#attributes' => array('class' => array('field-name')),
        ),
      ),
      'field_name' => array(
        '#type' => 'select',
        '#title' => t('Existing field to share'),
        '#title_display' => 'invisible',
        '#options' => $existing_field_options,
        '#empty_option' => t('- Select an existing field -'),
        '#description' => t('Field to share'),
        '#attributes' => array('class' => array('field-select')),
        '#cell_attributes' => array('colspan' => 2),
        '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
      ),
      'widget_type' => array(
        '#type' => 'select',
        '#title' => t('Widget for existing field'),
        '#title_display' => 'invisible',
        '#options' => $widget_type_options,
        '#empty_option' => t('- Select a widget -'),
        '#description' => t('Form element to edit the data.'),
        '#attributes' => array('class' => array('widget-type-select')),
        '#cell_attributes' => array('colspan' => 3),
        '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
      ),
    );
  }

  // Additional rows: add schema-based fields.
  $max_weight += 2;
  if ($field_type_options && $widget_type_options) {
    foreach ($schema['properties'] as $machine_name_root => $properties) {
      $max_weight +=1;
      $name = '_add_schema_field_' . $machine_name_root;
      $table[$name] = array(
        '#attributes' => array('class' => array('draggable', 'tabledrag-leaf', 'add-new')),
        '#row_type' => 'add_schema_field',
        '#region_callback' => 'entity_factory_field_overview_row_region',
        'insertion' => array(
          'add' => array(
            '#type' => 'checkbox',
            '#default_value' => FALSE,
          ),
          'rdf_predicates' => array(
            '#type' => 'hidden',
            '#default_value' => 'schema:' . $properties['field_name'],
          ),
        ),  
        'label' => array(
          '#type' => 'textfield',
          '#default_value' => ucfirst(str_replace('_', ' ', $machine_name_root)),
          '#title' => t('Schema field label'),
          '#title_display' => 'invisible',
          '#size' => 15,
          '#description' => t('Label'),
          '#prefix' => '<div class="label-input"><div class="add-new-placeholder">' . t('Add schema field') .'</div>',
          '#suffix' => '</div>',
        ),
        'weight' => array(
          '#type' => 'textfield',
          '#default_value' => $max_weight,
          '#size' => 3,
          '#title_display' => 'invisible',
          '#title' => t('Weight for schema field'),
          '#attributes' => array('class' => array('field-weight')),
          '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
        ),
        'parent_wrapper' => array(
          'parent' => array(
            '#type' => 'select',
            '#title' => t('Parent for schema field'),
            '#title_display' => 'invisible',
            '#options' => $table['#parent_options'],
            '#empty_value' => '',
            '#attributes' => array('class' => array('field-parent')),
            '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
            '#parents' => array('fields', $name, 'parent'),
          ),
          'hidden_name' => array(
            '#type' => 'hidden',
            '#default_value' => $name,
            '#attributes' => array('class' => array('field-name')),
          ),
        ),
        'field_name' => array(
          '#type' => 'machine_name',
          '#title' => t('Schema field name'),
          '#title_display' => 'invisible',
          // This field should stay LTR even for RTL languages.
          '#field_prefix' => '<span dir="ltr">field_',
          '#field_suffix' => '</span>&lrm;',
          '#size' => 15,
          '#description' => t('A unique machine-readable name containing letters, numbers, and underscores.'),
          // 32 characters minus the 'field_' prefix.
          '#maxlength' => 26,
          '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
          '#machine_name' => array(
            'source' => array('fields', $name, 'label'),
            'exists' => '_field_ui_field_name_exists',
            'standalone' => TRUE,
            'label' => '',
          ),
          '#required' => FALSE,
        ),
        'type' => array(
          '#type' => 'select',
//          '#title' => t('Type of schema field'),
          '#title' => t('Schema property type @proptype:', array('@proptype' => $properties['field_type'])),
          '#title_display' => 'invisible',
          '#options' => $field_type_options,
          '#empty_option' => t('- Select a field type -'),
          '#description' => t('Type of data to store.'),
          '#attributes' => array('class' => array('field-type-select')),
          '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
        ),
        'widget_type' => array(
          '#type' => 'select',
          '#title' => t('Widget for schema field'),
          '#title_display' => 'invisible',
          '#options' => $widget_type_options,
          '#empty_option' => t('- Select a widget -'),
          '#description' => t('Form element to edit the data.'),
          '#attributes' => array('class' => array('widget-type-select')),
//          '#cell_attributes' => array('colspan' => 2),
          '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
        ),
        'help_text' => array(
          '#type' => 'textarea',
          '#title' => t('Help text'),
          '#cell_attributes' => array('colspan' => 2),
          '#default_value' => $properties['description'],
        ),
        // Place the 'translatable' property as an explicit value so that contrib
        // modules can form_alter() the value for newly created fields.
        'translatable' => array(
          '#type' => 'value',
          '#value' => FALSE,
        ),
      );
    }
    
    
    
    
  }

  $form['fields'] = $table;


  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save'));

  $form['#attached']['css'][] = drupal_get_path('module', 'field_ui') . '/field_ui.css';
  $form['#attached']['js'][] = drupal_get_path('module', 'field_ui') . '/field_ui.js';

  // Add settings for the update selects behavior.
  $js_fields = array();
  foreach ($existing_fields as $field_name => $info) {
    $js_fields[$field_name] = array('label' => $info['label'], 'type' => $info['type'], 'widget' => $info['widget_type']);
  }

  $form['#attached']['js'][] = array(
    'type' => 'setting',
    'data' => array('fields' => $js_fields, 'fieldWidgetTypes' => field_ui_widget_type_options()),
  );

  // Add tabledrag behavior.
  $form['#attached']['drupal_add_tabledrag'][] = array('field-overview', 'order', 'sibling', 'field-weight');
  $form['#attached']['drupal_add_tabledrag'][] = array('field-overview', 'match', 'parent', 'field-parent', 'field-parent', 'field-name');

  return $form;
}

