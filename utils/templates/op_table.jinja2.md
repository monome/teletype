
<!--
make sure that the description column is long!
Pandoc uses the size of the header column to calculate the absolute column widths
(as Latex requires explict column sizes)
-->
| OP                     | OP _(set)_   | _(aliases)_ | Description                      |
|------------------------|--------------|-------------|----------------------------------|
{% for op in ops %}
| **`{{ op.prototype }}`** | {% if op.prototype_set is defined %}**`{{ op.prototype_set }}`**{% endif %} | {% for a in op.aliases %} **`{{a}}`** {% if not loop.last %}, {% endif %} {% endfor %} | {{ op.short | replace("\n", " ") }} |
{% endfor %}
